/*============================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

============================================================================*/
#include <unistd.h>
#include "camera_dbg.h"
#include "clf32.h"

#ifdef ENABLE_CLF_LOGGING
  #undef CDBG
  #define CDBG LOGE
#endif

#define IS_CLF_ENABLED(mod) (mod->cf_enable || mod->lf_enable)


/*===========================================================================
 * Function:           util_clf_luma_debug
 *
 * Description:
 *=========================================================================*/
static void util_clf_luma_debug(ISP_CLF_Luma_Update_CmdType* p_cmd)
{
  int i = 0;
  CDBG("%s: cutoff_1 %d cutoff_2 %d cutoff_3 %d", __func__,
    p_cmd->Cfg.cutoff_1, p_cmd->Cfg.cutoff_2,
    p_cmd->Cfg.cutoff_3);
  CDBG("%s: mult_neg %d mult_pos %d", __func__,
    p_cmd->Cfg.mult_neg, p_cmd->Cfg.mult_pos);

  for (i=0; i<8; i++)
    CDBG("%s: posLUT%d %d posLUT%d %d", __func__,
      2*i, p_cmd->pos_LUT[i].lut0,
      2*i+1, p_cmd->pos_LUT[i].lut1);

  for (i=0; i<4; i++)
    CDBG("%s: negLUT%d %d negLUT%d %d", __func__,
      2*i, p_cmd->neg_LUT[i].lut0,
      2*i+1, p_cmd->neg_LUT[i].lut1);
} /* util_clf_luma_debug */

/*===========================================================================
 * Function:           util_clf_chroma_debug
 *
 * Description:
 *=========================================================================*/
static void util_clf_chroma_debug(ISP_CLF_Chroma_Update_CmdType* p_cmd)
{
  int rc = TRUE;
  CDBG("%s: v_coeff0 %d v_coeff1 %d", __func__, p_cmd->chroma_coeff.v_coeff0,
     p_cmd->chroma_coeff.v_coeff1);

  CDBG("%s: h_coeff0 %d h_coeff1 %d h_coeff2 %d h_coeff3 %d", __func__,
       p_cmd->chroma_coeff.h_coeff0, p_cmd->chroma_coeff.h_coeff1,
       p_cmd->chroma_coeff.h_coeff2, p_cmd->chroma_coeff.h_coeff3);
} /* util_clf_chroma_debug */

/*===========================================================================
 * Function:           util_clf_debug
 *
 * Description:
 *=========================================================================*/
static void util_clf_debug(ISP_CLF_CmdType* p_cmd)
{
  CDBG("%s: colorconv_enable %d pipe_flush_cnt %d", __func__,
   p_cmd->clf_cfg.colorconv_enable,
   p_cmd->clf_cfg.pipe_flush_cnt);
  CDBG("%s: pipe_flush_ovd %d flush_halt_ovd %d", __func__,
   p_cmd->clf_cfg.pipe_flush_ovd,
   p_cmd->clf_cfg.flush_halt_ovd);

  util_clf_luma_debug(&p_cmd->lumaUpdateCmd);
  util_clf_chroma_debug(&p_cmd->chromaUpdateCmd);
} /* util_clf_debug */

/*===========================================================================
 * Function:           util_clf_luma_interpolate
 *
 * Description:
 *=========================================================================*/
static void util_clf_luma_interpolate(chromatix_adaptive_bayer_filter_data_type2 *in1,
  chromatix_adaptive_bayer_filter_data_type2 *in2,
  chromatix_adaptive_bayer_filter_data_type2 *out, float ratio)
{
  int i = 0;
  TBL_INTERPOLATE_INT(in1->threshold_red, in2->threshold_red, out->threshold_red,
    ratio, 3, i);

  for (i=0; i<16; i++)
    out->table_pos[i] =
      LINEAR_INTERPOLATION(in1->scale_factor_red[0] * in1->table_pos[i],
        in2->scale_factor_red[0] * in2->table_pos[i], ratio);

  for (i=0; i<8; i++)
    out->table_neg[i] =
      LINEAR_INTERPOLATION(in1->scale_factor_red[1] * in1->table_neg[i],
        in2->scale_factor_red[1] * in2->table_neg[i], ratio);

  /* setting scale factors to 1.0 since the curves are already calculated */
  for (i=0; i<2; i++)
    out->scale_factor_red[i] = 1.0;

} /* util_clf_luma_interpolate */

/*===========================================================================
 * Function:           util_clf_chroma_interpolate
 *
 * Description:
 *=========================================================================*/
static void util_clf_chroma_interpolate(Chroma_filter_type *in1,
  Chroma_filter_type *in2,
  Chroma_filter_type *out, float ratio)
{
  int i = 0;
  TBL_INTERPOLATE(in1->h, in2->h, out->h, ratio, 4, i);
  TBL_INTERPOLATE(in1->v, in2->v, out->v, ratio, 2, i);
} /* util_clf_chroma_interpolate */

/*===========================================================================
 * Function:           util_disable_luma
 *
 * Description:
 *=========================================================================*/
static void util_disable_luma(ISP_CLF_Luma_Update_CmdType* luma)
{
  memset(&luma->pos_LUT, 0x0, sizeof(ISP_CLF_Luma_Lut) * 8);
  memset(&luma->neg_LUT, 0x0, sizeof(ISP_CLF_Luma_Lut) * 4);
} /* util_disable_luma */

/*===========================================================================
 * Function:           util_disable_chroma
 *
 * Description:
 *=========================================================================*/
static void util_disable_chroma(ISP_CLF_Chroma_Update_CmdType* chroma)
{
  chroma->chroma_coeff.h_coeff0 = chroma->chroma_coeff.v_coeff0 = 1;
  chroma->chroma_coeff.h_coeff1 = chroma->chroma_coeff.h_coeff2 =
  chroma->chroma_coeff.h_coeff3 = chroma->chroma_coeff.v_coeff1 = 0;
} /* util_disable_chroma */

/*===========================================================================
 * Function:           util_clf_set_luma_params
 *
 * Description:
 *=========================================================================*/
static void util_clf_set_luma_params(isp_clf_mod_t *mod,
  chromatix_adaptive_bayer_filter_data_type2 *parm)
{
  int i = 0;
  int32_t temp;
  ISP_CLF_Luma_Update_CmdType* p_luma_cmd = &mod->reg_cmd.lumaUpdateCmd;

  if (!mod->lf_enable)
    return;

  p_luma_cmd->Cfg.cutoff_1 = ABF2_CUTOFF1(
    parm->threshold_red[0]);
  p_luma_cmd->Cfg.cutoff_2 = ABF2_CUTOFF2(
    p_luma_cmd->Cfg.cutoff_1,
  parm->threshold_red[1]);
  p_luma_cmd->Cfg.cutoff_3 = ABF2_CUTOFF3(
    p_luma_cmd->Cfg.cutoff_2,
  parm->threshold_red[2]);
  p_luma_cmd->Cfg.mult_neg = ABF2_MULT_NEG(
    p_luma_cmd->Cfg.cutoff_2,
  p_luma_cmd->Cfg.cutoff_3);
  p_luma_cmd->Cfg.mult_pos = ABF2_MULT_POS(
    p_luma_cmd->Cfg.cutoff_1);

  for (i = 0; i < 8; i++) {
    p_luma_cmd->pos_LUT[i].lut0 =
      ABF2_LUT(parm->table_pos[2*i] * parm->scale_factor_red[0]);
    p_luma_cmd->pos_LUT[i].lut1 =
      ABF2_LUT(parm->table_pos[2*i+1] * parm->scale_factor_red[0]);
  }

  for (i = 0; i < 4; i++) {
    p_luma_cmd->neg_LUT[i].lut0 =
      ABF2_LUT(parm->table_neg[2*i] * parm->scale_factor_red[1]);
    p_luma_cmd->neg_LUT[i].lut1 =
      ABF2_LUT(parm->table_neg[2*i+1] * parm->scale_factor_red[1]);
  }
} /* util_clf_set_luma_params */

/*===========================================================================
 * Function:           util_clf_set_chroma_parms
 *
 * Description:
 *=========================================================================*/
static void util_clf_set_chroma_params(isp_clf_mod_t* mod,
  Chroma_filter_type *parm, int8_t is_snap)
{
  ISP_CLF_Chroma_Update_CmdType* p_chroma_cmd = &mod->reg_cmd.chromaUpdateCmd;

  if (!mod->cf_enable)
    return;

  p_chroma_cmd->chroma_coeff.h_coeff0 = CLF_CF_COEFF(parm->h[0]);
  p_chroma_cmd->chroma_coeff.h_coeff1 = CLF_CF_COEFF(parm->h[1]);
  p_chroma_cmd->chroma_coeff.h_coeff2 = CLF_CF_COEFF(parm->h[2]);
  p_chroma_cmd->chroma_coeff.h_coeff3 = CLF_CF_COEFF(parm->h[3]);

  p_chroma_cmd->chroma_coeff.v_coeff0 = CLF_CF_COEFF(parm->v[0]);
  p_chroma_cmd->chroma_coeff.v_coeff1 = CLF_CF_COEFF(parm->v[1]);
} /* util_clf_set_chroma_parms */

/*===========================================================================
 * Function:           clf_chroma_trigger_update
 *
 * Description:
 *=========================================================================*/
static int clf_chroma_trigger_update(isp_clf_mod_t *mod,
  isp_pix_trigger_update_input_t *in_params, uint32_t in_param_size)
{
  chromatix_parms_type *chroma_ptr =
    (chromatix_parms_type *)in_params->cfg.chromatix_ptrs.chromatixPtr;
  chromatix_CL_filter_type *chromatix_CL_filter =
    &chroma_ptr->chromatix_VFE.chromatix_CL_filter;

  trigger_point_type  *p_trigger_point = NULL;
  int8_t is_burst = IS_BURST_STREAMING(&(in_params->cfg));
  tuning_control_type tuning_type;
  int8_t update_cf = FALSE;
  float ratio;
  Chroma_filter_type *p_cf_new = &(mod->clf_params.cf_param);
  Chroma_filter_type *p_cf = NULL;
  int8_t cf_enabled = mod->cf_enable && mod->cf_enable_trig;
  int status = 0;

  if (in_param_size != sizeof(isp_pix_trigger_update_input_t)) {
    /* size mismatch */
  CDBG_ERROR("%s: size mismatch, expecting = %d, received = %d",
      __func__, sizeof(isp_pix_trigger_update_input_t), in_param_size);
    return -1;
  }

  if (!cf_enabled) {
    CDBG("%s: Chroma filter trigger not enabled %d %d", __func__,
      mod->cf_enable, mod->cf_enable_trig);
    return 0;
  }

  mod->cf_update = FALSE;

  tuning_type = chromatix_CL_filter->control_chroma_filter;
  p_trigger_point = &chromatix_CL_filter->chroma_filter_trigger_lowlight;
  p_cf = chromatix_CL_filter->chroma_filter;

  ratio = isp_util_get_aec_ratio(mod->notify_ops->parent,
                                 tuning_type,
                                 p_trigger_point,
                                 &(in_params->trigger_input.
                                   stats_update.aec_update),
                                 is_burst);

  update_cf = (mod->old_streaming_mode != in_params->cfg.streaming_mode) ||
    !F_EQUAL(ratio, mod->cur_cf_aec_ratio);

  if (update_cf) {
    util_clf_chroma_interpolate(&p_cf[1], &p_cf[0], p_cf_new, ratio);
    mod->old_streaming_mode = in_params->cfg.streaming_mode;
    mod->cur_cf_aec_ratio = ratio;
    mod->cf_update = TRUE;
  }
  return status;
} /* clf_chroma_trigger_update */

/*===========================================================================
 * Function:           clf_luma_trigger_update
 *
 * Description:
 *=========================================================================*/
static int clf_luma_trigger_update(isp_clf_mod_t *mod,
  isp_pix_trigger_update_input_t *in_params, uint32_t in_param_size)
{
  trigger_point_type  *p_trigger_low = NULL, *p_trigger_bright = NULL;
  int8_t is_burst = IS_BURST_STREAMING(&(in_params->cfg));
  tuning_control_type tuning_type;
  int8_t update_lf = FALSE;
  float ratio;
  trigger_ratio_t trig_ratio;
  chromatix_parms_type *chroma_ptr =
    (chromatix_parms_type *)in_params->cfg.chromatix_ptrs.chromatixPtr;
  chromatix_ABF2_type *chromatix_ABF2 =
    &chroma_ptr->chromatix_VFE.chromatix_ABF2;
  int8_t lf_enabled = mod->lf_enable && mod->lf_enable_trig;
  int status = 0;

  if (!lf_enabled) {
    CDBG("%s: Luma filter trigger not enabled %d %d", __func__,
       mod->lf_enable, mod->lf_enable_trig);
    return status;
  }

  chromatix_adaptive_bayer_filter_data_type2 *p_lf_new =
    &(mod->clf_params.lf_param);
  chromatix_adaptive_bayer_filter_data_type2 *p_lf1 = NULL, *p_lf2 = NULL;

  mod->lf_update = FALSE;
  tuning_type = chromatix_ABF2->control_abf2;
  p_trigger_low = &chromatix_ABF2->abf2_low_light_trigger;
  p_trigger_bright = &chromatix_ABF2->abf2_bright_light_trigger;

  status =
    isp_util_get_aec_ratio2(mod->notify_ops->parent,
                            tuning_type,
                            p_trigger_bright,
                            p_trigger_low,
                            &(in_params->trigger_input.
                              stats_update.aec_update),
                            is_burst,
                            &trig_ratio);
  if (status != 0)
    CDBG_HIGH("%s: get aec ratio failed", __func__);

  switch (trig_ratio.lighting) {
  case TRIGGER_LOWLIGHT:
    p_lf1 = &chromatix_ABF2->abf2_config_low_light;
    p_lf2 = &chromatix_ABF2->abf2_config_normal_light;
    break;
  case TRIGGER_OUTDOOR:
    p_lf1 = &chromatix_ABF2->abf2_config_bright_light;
    p_lf2 = &chromatix_ABF2->abf2_config_normal_light;
    break;
  default:
  case TRIGGER_NORMAL:
    p_lf1 = p_lf2 = &chromatix_ABF2->abf2_config_normal_light;
    break;
  }

  update_lf =
   (trig_ratio.lighting != mod->cur_lf_aec_ratio.lighting) ||
   (trig_ratio.ratio != mod->cur_lf_aec_ratio.ratio) ||
   (in_params->cfg.streaming_mode != mod->old_streaming_mode);

  CDBG("%s: update_lf %d ratio %f lighting %d", __func__, update_lf,
    trig_ratio.ratio, trig_ratio.lighting);
  if (update_lf) {
    if(F_EQUAL(trig_ratio.ratio, 0.0) || F_EQUAL(trig_ratio.ratio, 1.0))
      *p_lf_new = *p_lf1;
    else
      util_clf_luma_interpolate(p_lf2, p_lf1, p_lf_new, trig_ratio.ratio);

    mod->cur_lf_aec_ratio = trig_ratio;
    mod->old_streaming_mode = in_params->cfg.streaming_mode; // need to handle for luma case
    mod->lf_update = TRUE;
  }
  return status;
} /* clf_luma_trigger_update */

/* ============================================================
 * function name: clf_init
 * description: init chroma luma filter module
 * ============================================================*/
static int clf_init (void *mod_ctrl, void *in_params, isp_notify_ops_t *notify_ops)
{
  isp_clf_mod_t *mod = mod_ctrl;
  isp_hw_mod_init_params_t *init_params = in_params;

  mod->fd = init_params->fd;
  mod->notify_ops = notify_ops;
  mod->old_streaming_mode = CAM_STREAMING_MODE_MAX;
  return 0;
} /* clf_init */

/*===========================================================================
 * FUNCTION    - clf_config -
 *
 * DESCRIPTION: configure initial settings
 *==========================================================================*/
static int clf_config(isp_clf_mod_t *mod, isp_hw_pix_setting_params_t *in_params, uint32_t in_param_size)
{
  int  rc = 0;
  chromatix_parms_type *chromatix_ptr =
    (chromatix_parms_type *)in_params->chromatix_ptrs.chromatixPtr;
  chromatix_ABF2_type *chromatix_ABF2 =
    &chromatix_ptr->chromatix_VFE.chromatix_ABF2;
  chromatix_CL_filter_type *chromatix_CL_filter =
    &chromatix_ptr->chromatix_VFE.chromatix_CL_filter;
  int is_burst;

  if (in_param_size != sizeof(isp_hw_pix_setting_params_t)) {
    /* size mismatch */
    CDBG_ERROR("%s: size mismatch, expecting = %d, received = %d",
      __func__, sizeof(isp_hw_pix_setting_params_t), in_param_size);
    return -1;
  }

  CDBG("%s: enter", __func__);

  if (!mod->enable) {
    CDBG("%s: Mod not Enable.", __func__);
    return rc;
  }

  if (!IS_CLF_ENABLED(mod)) {
    CDBG("%s: CLF not enabled", __func__);
    return 0;
  }

  is_burst = IS_BURST_STREAMING(in_params);
  /* set old cfg to invalid value to trigger the first trigger update */
  mod->old_streaming_mode = CAM_STREAMING_MODE_MAX;

  mod->cf_enable_trig = TRUE;
  mod->lf_enable_trig = TRUE;
  mod->cf_enable = TRUE;
  mod->lf_enable = TRUE;
  mod->trigger_enable = TRUE;

  mod->reg_cmd.clf_cfg.colorconv_enable = (uint32_t) in_params->camif_cfg.is_bayer_sensor;
  mod->reg_cmd.clf_cfg.flush_halt_ovd = 0;
  mod->reg_cmd.clf_cfg.pipe_flush_cnt = 0x400;
  mod->reg_cmd.clf_cfg.pipe_flush_ovd = 1;

  /*preview*/
  util_clf_set_luma_params(mod, &chromatix_ABF2->abf2_config_normal_light);
  util_clf_set_chroma_params(mod, &chromatix_CL_filter->chroma_filter[1],
    is_burst);

  //originally config
  if (is_burst) {
    ISP_CLF_CmdType* ISP_CLF_Cmd = &mod->reg_cmd;

    chromatix_adaptive_bayer_filter_data_type2* p_lf =
      (mod->lf_update) ?
      &mod->clf_params.lf_param :
      &chromatix_ABF2->abf2_config_normal_light;

    Chroma_filter_type* p_cf =
      (mod->cf_update) ?
      &mod->clf_params.cf_param :
      &chromatix_CL_filter->chroma_filter[1];

    CDBG("%s: lf_update %d cf_update %d", __func__, mod->lf_update,
       mod->cf_update);

    util_clf_set_luma_params(mod, p_lf);
    util_clf_set_chroma_params(mod, p_cf, is_burst);

    CDBG("CLF Burst shot config");
    util_clf_debug(ISP_CLF_Cmd);
  }
  mod->skip_trigger = FALSE;
  mod->hw_update_pending = TRUE;

  return rc;
} /* clf_config */

/* ============================================================
 * function name: clf_destroy
 * description: close mod
 * ============================================================*/
static int clf_destroy (void *mod_ctrl)
{
  isp_clf_mod_t *mod = mod_ctrl;

  memset(mod,  0,  sizeof(isp_clf_mod_t));
  free(mod);
  return 0;
} /* clf_destroy */

/* ============================================================
 * function name: clf_enable
 * description: enable module
 * ============================================================*/
static int clf_enable(isp_clf_mod_t *mod,
  isp_mod_set_enable_t *enable, uint32_t in_param_size)
{
  if (in_param_size != sizeof(isp_mod_set_enable_t)) {
    /* size mismatch */
  CDBG_ERROR("%s: size mismatch, expecting = %d, received = %d",
      __func__, sizeof(isp_mod_set_enable_t), in_param_size);
    return -1;
  }

  mod->lf_enable = mod->cf_enable = mod->enable = enable->enable;

  int clf_enable = IS_CLF_ENABLED(mod);
  CDBG("%s: clf_enable %d", __func__, clf_enable);

  if (!clf_enable)
    /* disable both modules */
    mod->lf_enable = mod->cf_enable = FALSE;
  else
    mod->lf_enable = mod->cf_enable = TRUE;

  if (!mod->enable)
      mod->hw_update_pending = 0;
  return 0;
} /* clf_enable */

/* ============================================================
 * function name: clf_trigger_enable
 * description: enable trigger update feature
 * ============================================================*/
static int clf_trigger_enable(isp_clf_mod_t *mod,
  isp_mod_set_enable_t *enable, uint32_t in_param_size)
{
  if (in_param_size != sizeof(isp_mod_set_enable_t)) {
    CDBG_ERROR("%s: size mismatch, expecting = %d, received = %d",
      __func__, sizeof(isp_mod_set_enable_t), in_param_size);
    return -1;
  }
  mod->trigger_enable = enable->enable;
  return 0;
} /* clf_trigger_enable */

/*===========================================================================
 * FUNCTION    - clf_trigger_update
 *
 * DESCRIPTION:
 *==========================================================================*/
static int clf_trigger_update(isp_clf_mod_t *mod,
  isp_pix_trigger_update_input_t *in_params, uint32_t in_param_size)
{
  int rc = 0;
  ISP_CLF_CmdType *reg_cmd = &mod->reg_cmd;
  chromatix_parms_type *chromatix_ptr =
    in_params->cfg.chromatix_ptrs.chromatixPtr;
  trigger_point_type *p_trigger_point;
  float aec_ratio;
  cs_luma_threshold_type *cs_luma_threshold, *cs_luma_threshold_lowlight;
  uint8_t update_clf = FALSE;
  int is_burst;
  int status = 0;

  if (in_param_size != sizeof(isp_pix_trigger_update_input_t)) {
  /* size mismatch */
  CDBG_ERROR("%s: size mismatch, expecting = %d, received = %d",
         __func__, sizeof(isp_pix_trigger_update_input_t), in_param_size);
  return -1;
  }

  if (!mod->enable || !mod->trigger_enable || mod->skip_trigger) {
    CDBG("%s: Skip Trigger update. enable %d, trig_enable %d, skip_trigger %d",
      __func__, mod->enable, mod->trigger_enable, mod->skip_trigger);
    return rc;
  }

  is_burst = IS_BURST_STREAMING(&in_params->cfg);

  if (!is_burst && !isp_util_aec_check_settled(&in_params->trigger_input.stats_update.aec_update)) {
    CDBG("%s: AEC not settled", __func__);
    return rc;
  }

    status = clf_luma_trigger_update(mod, in_params, in_param_size);
  if (0 != status) {
    CDBG("%s: isp_clf_luma_trigger_update failed", __func__);
    return status;
  }
  status = clf_chroma_trigger_update(mod, in_params, in_param_size);
  if (0 != status) {
    CDBG("%s: isp_clf_luma_trigger_update failed", __func__);
    return status;
  }

  //update:
  if (mod->lf_update && !mod->lf_enable) {
    CDBG("%s: update not required", __func__);

    util_clf_set_luma_params(mod, &mod->clf_params.lf_param);
    CDBG("CLF update");
    util_clf_debug(&(mod->reg_cmd));

    mod->hw_update_pending = TRUE;
  }

  if (mod->cf_update && mod->cf_enable) {
    CDBG("%s: update not required", __func__);

    util_clf_set_chroma_params(mod, &mod->clf_params.cf_param, is_burst);

    mod->hw_update_pending = TRUE;
  }

  mod->hw_update_pending = TRUE;
  return 0;
} /* clf_trigger_update */

/* ============================================================
 * function name: clf_set_chromatix
 * description: set chromatix info to modules
 * ============================================================*/
static int clf_set_chromatix(isp_clf_mod_t *mod,
  isp_hw_pix_setting_params_t *in_params, uint32_t in_param_size)
{
  if (in_param_size != sizeof(isp_hw_pix_setting_params_t)) {
  /* size mismatch */
  CDBG_ERROR("%s: size mismatch, expecting = %d, received = %d",
         __func__, sizeof(isp_hw_pix_setting_params_t), in_param_size);
  return -1;
  }
  chromatix_parms_type * chromatix_ptr =
    (chromatix_parms_type *) in_params->chromatix_ptrs.chromatixPtr;
  chromatix_ABF2_type *chromatix_ABF2 =
    &chromatix_ptr->chromatix_VFE.chromatix_ABF2;
  chromatix_CL_filter_type *chromatix_CL_filter =
    &chromatix_ptr->chromatix_VFE.chromatix_CL_filter;
  int is_burst = IS_BURST_STREAMING(in_params);

    /*snapshot*/
    util_clf_set_luma_params(mod,
    &chromatix_ABF2->abf2_config_normal_light);
    util_clf_set_chroma_params(mod,
    &chromatix_CL_filter->chroma_filter[1], is_burst);

  mod->skip_trigger = FALSE;

  return 0;
} /* clf_set_chromatix */

/* ============================================================
 * function name: clf_hw_reg_update
 * description: update module register to kernel
 * ============================================================*/
static int clf_do_hw_update(isp_clf_mod_t *clf_mod)
{
  int rc = 0;
  struct msm_vfe_cfg_cmd2 cfg_cmd;
  struct msm_vfe_reg_cfg_cmd reg_cfg_cmd[1];

  if (clf_mod->hw_update_pending) {
    cfg_cmd.cfg_data = (void *) &clf_mod->reg_cmd;
    cfg_cmd.cmd_len = sizeof(clf_mod->reg_cmd);
    cfg_cmd.cfg_cmd = (void *) reg_cfg_cmd;
    cfg_cmd.num_cfg = 1;

    reg_cfg_cmd[0].u.rw_info.cmd_data_offset = 0;
    reg_cfg_cmd[0].cmd_type = VFE_WRITE;
    reg_cfg_cmd[0].u.rw_info.reg_offset = ISP_CLF32_CFG_OFF;
    reg_cfg_cmd[0].u.rw_info.len = ISP_CLF32_CFG_LEN * sizeof(uint32_t);

    util_clf_debug(&clf_mod->reg_cmd);
    rc = ioctl(clf_mod->fd, VIDIOC_MSM_VFE_REG_CFG, &cfg_cmd);
    if (rc < 0){
      CDBG_ERROR("%s: HW update error, rc = %d", __func__, rc);
      return rc;
    }
    memcpy(&(clf_mod->applied_clf_params),&(clf_mod->clf_params),
     sizeof(clf_params_t));
    clf_mod->hw_update_pending = 0;
  }

  /* TODO: update hw reg */
  return rc;
} /* clf_hw_reg_update */

/* ============================================================
 * function name: clf_set_params
 * description: set parameters from ISP
 * ============================================================*/
static int clf_set_params (
  void *mod_ctrl, uint32_t param_id,
  void *in_params, uint32_t in_param_size)
{
  isp_clf_mod_t *mod = mod_ctrl;
  int rc = 0;

  switch (param_id) {
  case ISP_HW_MOD_SET_CHROMATIX_RELOAD:
  rc = clf_set_chromatix(mod,
      (isp_hw_pix_setting_params_t *) in_params, in_param_size);
  break;
  case ISP_HW_MOD_SET_MOD_ENABLE:
  rc = clf_enable(mod,
      (isp_mod_set_enable_t *) in_params, in_param_size);
  break;
  case ISP_HW_MOD_SET_MOD_CONFIG:
  rc = clf_config(mod,
      (isp_hw_pix_setting_params_t *) in_params, in_param_size);
  break;
  case ISP_HW_MOD_SET_TRIGGER_ENABLE:
  rc = clf_trigger_enable(mod,
      (isp_mod_set_enable_t *) in_params, in_param_size);
  break;
  case ISP_HW_MOD_SET_TRIGGER_UPDATE:
  rc = clf_trigger_update(mod,
      (isp_pix_trigger_update_input_t *)in_params, in_param_size);
  break;
  default:
    CDBG_ERROR("%s: param_id %d, is not supported in this module\n", __func__, param_id);
    break;
  }
  return rc;
} /* clf_set_params */

static void clf_ez_isp_update(
 isp_clf_mod_t *clf_mod,
 chromalumafiltercoeff_t *clf_Diag)
{
  memcpy(clf_Diag, &(clf_mod->applied_clf_params),
   sizeof(chromalumafiltercoeff_t));
}

/* ============================================================
 * function name: clf_get_params
 * description: get parameters
 * ============================================================*/
static int clf_get_params (
  void *mod_ctrl, uint32_t param_id,
  void *in_params, uint32_t in_param_size,
  void *out_params, uint32_t out_param_size)
{
  isp_clf_mod_t *mod = mod_ctrl;
  int rc =0;

  switch (param_id) {
  case ISP_HW_MOD_GET_MOD_ENABLE: {
    isp_mod_get_enable_t *enable = out_params;

    if (sizeof(isp_mod_get_enable_t) != out_param_size) {
      CDBG_ERROR("%s: error, out_param_size mismatch, param_id = %d",
                 __func__, param_id);
      break;
    }
    enable->enable = mod->enable;
    break;
  }

  case ISP_HW_MOD_GET_VFE_DIAG_INFO_USER: {
    vfe_diagnostics_t *vfe_diag = (vfe_diagnostics_t *)out_params;
    chromalumafiltercoeff_t *clf_diag;
    if (sizeof(vfe_diagnostics_t) != out_param_size) {
      CDBG_ERROR("%s: error, out_param_size mismatch, param_id = %d",
        __func__, param_id);
      break;
    }
    clf_diag = &(vfe_diag->prev_chromalumafilter);
    if (mod->old_streaming_mode == CAM_STREAMING_MODE_BURST) {
      clf_diag = &(vfe_diag->snap_chromalumafilter);
    }
    vfe_diag->control_clfilter.enable = mod->enable;
    vfe_diag->control_clfilter.cntrlenable = mod->trigger_enable;
    clf_ez_isp_update(mod, clf_diag);
    /*Populate vfe_diag data*/
    CDBG("%s: Populating vfe_diag data", __func__);
  }
    break;
  default:
    rc = -1;
    break;
  }
  return 0;
} /* clf_get_params */

/* ============================================================
 * function name: clf_action
 * description: processing the action
 * ============================================================*/
static int clf_action (void *mod_ctrl,
 uint32_t action_code,
                 void *data, uint32_t data_size)
{
  int rc = 0;
  isp_clf_mod_t *mod = mod_ctrl;

  switch (action_code) {
  case ISP_HW_MOD_ACTION_HW_UPDATE:
    rc = clf_do_hw_update(mod);
    break;
  default:
    /* no op */
    CDBG_HIGH("%s: action code = %d is not supported. nop",
      __func__, action_code);
    rc = -EAGAIN;
    break;
  }
  return rc;
} /* clf_action */

/* ============================================================
 * function name: clf32_open
 * description: open color_correct32 mod, create func table
 * ============================================================*/
isp_ops_t *clf32_open(uint32_t version)
{
  isp_clf_mod_t *mod = malloc(sizeof(isp_clf_mod_t));

  CDBG("%s: E", __func__);

  if (!mod) {
    CDBG_ERROR("%s: fail to allocate memory",  __func__);
    return NULL;
  }
  memset(mod,  0,  sizeof(isp_clf_mod_t));

  mod->ops.ctrl = (void *)mod;
  mod->ops.init = clf_init;
  mod->ops.destroy = clf_destroy;
  mod->ops.set_params = clf_set_params;
  mod->ops.get_params = clf_get_params;
  mod->ops.action = clf_action;

  return &mod->ops;
} /* clf32_open */
