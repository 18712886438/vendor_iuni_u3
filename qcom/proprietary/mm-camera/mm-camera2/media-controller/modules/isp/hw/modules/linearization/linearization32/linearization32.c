/*============================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

============================================================================*/
#include <unistd.h>
#include <math.h>
#include "camera_dbg.h"
#include "linearization32.h"

#ifdef ENABLE_LINEAR_LOGGING
  #undef CDBG
  #define CDBG LOGE
#endif


static const char * const aec_debug_str[] = {
  "LINEAR_AEC_BRIGHT",
  "LINEAR_AEC_BRIGHT_NORMAL",
  "LINEAR_AEC_NORMAL",
  "LINEAR_AEC_NORMAL_LOW",
  "LINEAR_AEC_LOW",
  "LINEAR_AEC_LUX_MAX",
};

static const char * const awb_debug_str[] = {
  "AWB_CCT_TYPE_D65",
  "AWB_CCT_TYPE_D65_TL84",
  "AWB_CCT_TYPE_TL84",
  "AWB_CCT_TYPE_TL84_A",
  "AWB_CCT_TYPE_A",
  "AWB_CCT_TYPE_MAX",
};
/*===========================================================================
 * FUNCTION    - print_vfe_linearization_config -
 *
 * DESCRIPTION:
 *==========================================================================*/
static void print_isp_linearization_config(ISP_LinearizationCmdType *cmd)
{
  int i;
  uint32_t *ptr;

  CDBG("%s: Linearization configurations\n", __func__);

  CDBG("pointSlopeR.kneePoint_P0 = %d\n",
    cmd->CfgParams.pointSlopeR.kneePoint_P0);
  CDBG("pointSlopeR.kneePoint_P1 = %d\n",
    cmd->CfgParams.pointSlopeR.kneePoint_P1);
  CDBG("pointSlopeR.kneePoint_P2 = %d\n",
    cmd->CfgParams.pointSlopeR.kneePoint_P2);
  CDBG("pointSlopeR.kneePoint_P3 = %d\n",
    cmd->CfgParams.pointSlopeR.kneePoint_P3);
  CDBG("pointSlopeR.kneePoint_P4 = %d\n",
    cmd->CfgParams.pointSlopeR.kneePoint_P4);
  CDBG("pointSlopeR.kneePoint_P5 = %d\n",
    cmd->CfgParams.pointSlopeR.kneePoint_P5);
  CDBG("pointSlopeR.kneePoint_P6 = %d\n",
    cmd->CfgParams.pointSlopeR.kneePoint_P6);
  CDBG("pointSlopeR.kneePoint_P7 = %d\n",
    cmd->CfgParams.pointSlopeR.kneePoint_P7);

  CDBG("pointSlopeGb.kneePoint_P0 = %d\n",
    cmd->CfgParams.pointSlopeGb.kneePoint_P0);
  CDBG("pointSlopeGb.kneePoint_P1 = %d\n",
    cmd->CfgParams.pointSlopeGb.kneePoint_P1);
  CDBG("pointSlopeGb.kneePoint_P2 = %d\n",
    cmd->CfgParams.pointSlopeGb.kneePoint_P2);
  CDBG("pointSlopeGb.kneePoint_P3 = %d\n",
    cmd->CfgParams.pointSlopeGb.kneePoint_P3);
  CDBG("pointSlopeGb.kneePoint_P4 = %d\n",
    cmd->CfgParams.pointSlopeGb.kneePoint_P4);
  CDBG("pointSlopeGb.kneePoint_P5 = %d\n",
    cmd->CfgParams.pointSlopeGb.kneePoint_P5);
  CDBG("pointSlopeGb.kneePoint_P6 = %d\n",
    cmd->CfgParams.pointSlopeGb.kneePoint_P6);
  CDBG("pointSlopeGb.kneePoint_P7 = %d\n",
    cmd->CfgParams.pointSlopeGb.kneePoint_P7);

  CDBG("pointSlopeB.kneePoint_P0 = %d\n",
    cmd->CfgParams.pointSlopeB.kneePoint_P0);
  CDBG("pointSlopeB.kneePoint_P1 = %d\n",
    cmd->CfgParams.pointSlopeB.kneePoint_P1);
  CDBG("pointSlopeB.kneePoint_P2 = %d\n",
    cmd->CfgParams.pointSlopeB.kneePoint_P2);
  CDBG("pointSlopeB.kneePoint_P3 = %d\n",
    cmd->CfgParams.pointSlopeB.kneePoint_P3);
  CDBG("pointSlopeB.kneePoint_P4 = %d\n",
    cmd->CfgParams.pointSlopeB.kneePoint_P4);
  CDBG("pointSlopeB.kneePoint_P5 = %d\n",
    cmd->CfgParams.pointSlopeB.kneePoint_P5);
  CDBG("pointSlopeB.kneePoint_P6 = %d\n",
    cmd->CfgParams.pointSlopeB.kneePoint_P6);
  CDBG("pointSlopeB.kneePoint_P7 = %d\n",
    cmd->CfgParams.pointSlopeB.kneePoint_P7);

  CDBG("pointSlopeGr.kneePoint_P0 = %d\n",
    cmd->CfgParams.pointSlopeGr.kneePoint_P0);
  CDBG("pointSlopeGr.kneePoint_P1 = %d\n",
    cmd->CfgParams.pointSlopeGr.kneePoint_P1);
  CDBG("pointSlopeGr.kneePoint_P2 = %d\n",
    cmd->CfgParams.pointSlopeGr.kneePoint_P2);
  CDBG("pointSlopeGr.kneePoint_P3 = %d\n",
    cmd->CfgParams.pointSlopeGr.kneePoint_P3);
  CDBG("pointSlopeGr.kneePoint_P4 = %d\n",
    cmd->CfgParams.pointSlopeGr.kneePoint_P4);
  CDBG("pointSlopeGr.kneePoint_P5 = %d\n",
    cmd->CfgParams.pointSlopeGr.kneePoint_P5);
  CDBG("pointSlopeGr.kneePoint_P6 = %d\n",
    cmd->CfgParams.pointSlopeGr.kneePoint_P6);
  CDBG("pointSlopeGr.kneePoint_P7 = %d\n",
    cmd->CfgParams.pointSlopeGr.kneePoint_P7);

} /* print_vfe_linearization_config */

/*===========================================================================
 * FUNCTION    - vfe_convert_linearization_tbl -
 *
 * DESCRIPTION:
 *==========================================================================*/
static void convert_linearization_tbl(chromatix_linearization_type *tbl,
  ISP_LinearizationLut *pBlkTbl)
{
  uint16_t i = 0;

  for ( i = 0 ; i < 8 ; i++ ) {
    pBlkTbl->r_lut_p[i] = tbl->r_lut_p[i];
    pBlkTbl->gr_lut_p[i] = tbl->gr_lut_p[i];
    pBlkTbl->gb_lut_p[i] = tbl->gb_lut_p[i];
    pBlkTbl->b_lut_p[i] = tbl->b_lut_p[i];
  }

  for ( i = 0 ; i < 9 ; i++ ) {
    pBlkTbl->r_lut_base[i] = tbl->r_lut_base[i];
    pBlkTbl->gr_lut_base[i] = tbl->gr_lut_base[i];
    pBlkTbl->gb_lut_base[i] = tbl->gb_lut_base[i];
    pBlkTbl->b_lut_base[i] = tbl->b_lut_base[i];

    pBlkTbl->r_lut_delta[i] = FLOAT_TO_Q(9,tbl->r_lut_delta[i]);
    pBlkTbl->gr_lut_delta[i] = FLOAT_TO_Q(9,tbl->gr_lut_delta[i]);
    pBlkTbl->gb_lut_delta[i] = FLOAT_TO_Q(9,tbl->gb_lut_delta[i]);
    pBlkTbl->b_lut_delta[i] = FLOAT_TO_Q(9,tbl->b_lut_delta[i]);
  }
} /* vfe_convert_linearization_tbl */

/*===========================================================================
 * FUNCTION    - vfe_write_linearization_table -
 *
 * DESCRIPTION:
 *==========================================================================*/
static void write_linearization_table(const ISP_LinearizationLut *pIn,
  ISP_LinearizationCmdType *blkConfigCmd)
{
  uint16_t i,j;
  uint32_t data = 0;

  const uint16_t* baseR = pIn->r_lut_base;
  const uint16_t* baseGr = pIn->gr_lut_base;
  const uint16_t* baseGb = pIn->gb_lut_base;
  const uint16_t* baseB = pIn->b_lut_base;

  const uint32_t* sR = pIn->r_lut_delta;
  const uint32_t* sGr = pIn->gr_lut_delta;
  const uint32_t* sGb = pIn->gb_lut_delta;
  const uint32_t* sB = pIn->b_lut_delta;

  /* pack the table in HW specific arrangement */
  for (i=0, j=0; i<ISP32_LINEARIZATON_TABLE_LENGTH/4; i++, j=j+4) {
    blkConfigCmd->CfgTbl.Lut[j] = ((((uint32_t)(*baseR)) & 0x00000FFF) |
      ((*sR & 0x0003FFFF) << 12));
    baseR++;
    sR++;

    blkConfigCmd->CfgTbl.Lut[j+1] = ((((uint32_t)(*baseGr)) & 0x00000FFF) |
      ((*sGr & 0x0003FFFF) << 12));
    baseGr++;
    sGr++;

    blkConfigCmd->CfgTbl.Lut[j+2] = ((((uint32_t)(*baseGb)) & 0x00000FFF) |
      ((*sGb & 0x0003FFFF) << 12));
    baseGb++;
    sGb++;

    blkConfigCmd->CfgTbl.Lut[j+3] = ((((uint32_t)(*baseB)) & 0x00000FFF) |
      ((*sB & 0x0003FFFF) << 12));
    baseB++;
    sB++;
  }
}/* vfe_write_linearization_table */

/*===========================================================================
 * FUNCTION    - vfe_config_linearization_cmd -
 *
 * DESCRIPTION:
 *==========================================================================*/
static void config_linearization_cmd(
  isp_linear_mod_t *mod,
  chromatix_linearization_type *pTbl)
{
  int i;
  uint32_t *ptr;
  ISP_LinearizationCmdType *cmd;

  convert_linearization_tbl(pTbl, &(mod->linear_lut));

  cmd =&(mod->linear_cmd);

  /* Configure the Linearization */
  cmd->CfgParams.pointSlopeR.kneePoint_P0 = mod->linear_lut.r_lut_p[0];
  cmd->CfgParams.pointSlopeR.kneePoint_P1 = mod->linear_lut.r_lut_p[1];
  cmd->CfgParams.pointSlopeR.kneePoint_P2 = mod->linear_lut.r_lut_p[2];
  cmd->CfgParams.pointSlopeR.kneePoint_P3 = mod->linear_lut.r_lut_p[3];
  cmd->CfgParams.pointSlopeR.kneePoint_P4 = mod->linear_lut.r_lut_p[4];
  cmd->CfgParams.pointSlopeR.kneePoint_P5 = mod->linear_lut.r_lut_p[5];
  cmd->CfgParams.pointSlopeR.kneePoint_P6 = mod->linear_lut.r_lut_p[6];
  cmd->CfgParams.pointSlopeR.kneePoint_P7 = mod->linear_lut.r_lut_p[7];

  cmd->CfgParams.pointSlopeGb.kneePoint_P0 = mod->linear_lut.gb_lut_p[0];
  cmd->CfgParams.pointSlopeGb.kneePoint_P1 = mod->linear_lut.gb_lut_p[1];
  cmd->CfgParams.pointSlopeGb.kneePoint_P2 = mod->linear_lut.gb_lut_p[2];
  cmd->CfgParams.pointSlopeGb.kneePoint_P3 = mod->linear_lut.gb_lut_p[3];
  cmd->CfgParams.pointSlopeGb.kneePoint_P4 = mod->linear_lut.gb_lut_p[4];
  cmd->CfgParams.pointSlopeGb.kneePoint_P5 = mod->linear_lut.gb_lut_p[5];
  cmd->CfgParams.pointSlopeGb.kneePoint_P6 = mod->linear_lut.gb_lut_p[6];
  cmd->CfgParams.pointSlopeGb.kneePoint_P7 = mod->linear_lut.gb_lut_p[7];

  cmd->CfgParams.pointSlopeB.kneePoint_P0 = mod->linear_lut.b_lut_p[0];
  cmd->CfgParams.pointSlopeB.kneePoint_P1 = mod->linear_lut.b_lut_p[1];
  cmd->CfgParams.pointSlopeB.kneePoint_P2 = mod->linear_lut.b_lut_p[2];
  cmd->CfgParams.pointSlopeB.kneePoint_P3 = mod->linear_lut.b_lut_p[3];
  cmd->CfgParams.pointSlopeB.kneePoint_P4 = mod->linear_lut.b_lut_p[4];
  cmd->CfgParams.pointSlopeB.kneePoint_P5 = mod->linear_lut.b_lut_p[5];
  cmd->CfgParams.pointSlopeB.kneePoint_P6 = mod->linear_lut.b_lut_p[6];
  cmd->CfgParams.pointSlopeB.kneePoint_P7 = mod->linear_lut.b_lut_p[7];

  cmd->CfgParams.pointSlopeGr.kneePoint_P0 = mod->linear_lut.gr_lut_p[0];
  cmd->CfgParams.pointSlopeGr.kneePoint_P1 = mod->linear_lut.gr_lut_p[1];
  cmd->CfgParams.pointSlopeGr.kneePoint_P2 = mod->linear_lut.gr_lut_p[2];
  cmd->CfgParams.pointSlopeGr.kneePoint_P3 = mod->linear_lut.gr_lut_p[3];
  cmd->CfgParams.pointSlopeGr.kneePoint_P4 = mod->linear_lut.gr_lut_p[4];
  cmd->CfgParams.pointSlopeGr.kneePoint_P5 = mod->linear_lut.gr_lut_p[5];
  cmd->CfgParams.pointSlopeGr.kneePoint_P6 = mod->linear_lut.gr_lut_p[6];
  cmd->CfgParams.pointSlopeGr.kneePoint_P7 = mod->linear_lut.gr_lut_p[7];

  write_linearization_table(&(mod->linear_lut), cmd);

  print_isp_linearization_config(cmd);
}/* vfe_config_linearization_cmd */

/*===========================================================================
 * FUNCTION    - trigger_interpolate_linear_table -
 *
 * DESCRIPTION:
 *==========================================================================*/
static void trigger_interpolate_linear_table(
  chromatix_linearization_type *input1, chromatix_linearization_type*input2,
  float ratio, chromatix_linearization_type *output)
{
  int i;

  // Interpolate the knee points (x - cooridnate)
  TBL_INTERPOLATE(input1->r_lut_p, input2->r_lut_p, output->r_lut_p,
    ratio, 8, i);
  TBL_INTERPOLATE(input1->gr_lut_p, input2->gr_lut_p, output->gr_lut_p,
    ratio, 8, i);
  TBL_INTERPOLATE(input1->gb_lut_p, input2->gb_lut_p, output->gb_lut_p,
    ratio, 8, i);
  TBL_INTERPOLATE(input1->b_lut_p, input2->b_lut_p, output->b_lut_p,
    ratio, 8, i);

  // Interpolate the base co-ordinates (y - cooridnate)
  TBL_INTERPOLATE(input1->r_lut_base, input2->r_lut_base, output->r_lut_base,
    ratio, 9, i);
  TBL_INTERPOLATE(input1->gr_lut_base, input2->gr_lut_base, output->gr_lut_base,
    ratio, 9, i);
  TBL_INTERPOLATE(input1->gb_lut_base, input2->gb_lut_base, output->gb_lut_base,
    ratio, 9, i);
  TBL_INTERPOLATE(input1->b_lut_base, input2->b_lut_base, output->b_lut_base,
    ratio, 9, i);

  /* Calculate Slope from knee point co-ordinates, no interpolation
     p[1]-p[0] / base[1] - base[0]
     first knee point = 0
     Last knee point and base(in normal light) = 4095
     first slope = 0
  */

  // All segment zero slopes are zero
  output->r_lut_delta[0] = 0.0;
  output->gr_lut_delta[0] = 0.0;
  output->gb_lut_delta[0] = 0.0;
  output->b_lut_delta[0] = 0.0;

  // calculate slope for segements 1 - 7
  for (i = 1 ; i < 8 ; i++) {
    output->r_lut_delta[i] = CALC_SLOPE(output->r_lut_p[i-1],
      output->r_lut_p[i], output->r_lut_base[i], output->r_lut_base[i+1]);

    output->gr_lut_delta[i] = CALC_SLOPE(output->gr_lut_p[i-1],
      output->gr_lut_p[i], output->gr_lut_base[i], output->gr_lut_base[i+1]);

    output->gb_lut_delta[i] = CALC_SLOPE(output->gb_lut_p[i-1],
      output->gb_lut_p[i], output->gb_lut_base[i], output->gb_lut_base[i+1]);

    output->b_lut_delta[i] = CALC_SLOPE(output->b_lut_p[i-1],
      output->b_lut_p[i], output->b_lut_base[i], output->b_lut_base[i+1]);
  }

  // calculate slope for  last segements, last base and knee points = 4095
  // R channel
  output->r_lut_delta[8] = CALC_SLOPE(output->r_lut_p[7], 4095,
    output->r_lut_base[8], 4095);
  // GR channel
  output->gr_lut_delta[8] = CALC_SLOPE(output->gr_lut_p[7], 4095,
    output->gr_lut_base[8], 4095);
  // GB channel
  output->gb_lut_delta[8] = CALC_SLOPE(output->gb_lut_p[7], 4095,
    output->gb_lut_base[8], 4095);
  // B channel
  output->b_lut_delta[8] = CALC_SLOPE(output->b_lut_p[7], 4095,
    output->b_lut_base[8], 4095);

} /* trigger_interpolate_linear_table */

/*===========================================================================
 * FUNCTION    - isp_linear_update_lowlight_table -
 *
 * DESCRIPTION:
 *==========================================================================*/
static void linear_update_lowlight_table(chromatix_linearization_type *ip,
  chromatix_linearization_type *op, float ratio, uint16_t max_blk_increase)
{
  int i = 0;
  float virtual_slope;
  uint32_t virtual_base;

  for (i = 0; i < 8; i++) {
    op->r_lut_p[i] = (ip->r_lut_p[i] - (1.0 - ratio) * max_blk_increase);
    op->gr_lut_p[i] = (ip->gr_lut_p[i] - (1.0 - ratio) * max_blk_increase);
    op->gb_lut_p[i] = (ip->gb_lut_p[i] - (1.0 - ratio) * max_blk_increase);
    op->b_lut_p[i] = (ip->b_lut_p[i] - (1.0 - ratio) * max_blk_increase);
  }

  for (i = 0 ; i < 9 ; i++) {
    op->r_lut_base[i] = ip->r_lut_base[i];
    op->gr_lut_base[i] = ip->gr_lut_base[i];
    op->gb_lut_base[i] = ip->gb_lut_base[i];
    op->b_lut_base[i] = ip->b_lut_base[i];
  }

  op->r_lut_delta[0] = 0.0;
  op->gr_lut_delta[0] = 0.0;
  op->gb_lut_delta[0] = 0.0;
  op->b_lut_delta[0] = 0.0;

  for (i = 1 ; i < 8 ; i++) {
    op->r_lut_delta[i] = ip->r_lut_delta[i];
    op->gr_lut_delta[i] = ip->gr_lut_delta[i];
    op->gb_lut_delta[i] = ip->gb_lut_delta[i];
    op->b_lut_delta[i] = ip->b_lut_delta[i];
  }

  /*
    calculate the last segment slope separately, as the curve
    shifts left, there is chance for last base point to exceed
    4095, hence re calculate last segment slope after clamping
    the last base point to 4095
  */
  // calculate the virtual base point
  virtual_base = ip->r_lut_base[8] +
      (ip->r_lut_delta[8]) * (4095 - ip->r_lut_p[7]);
  CDBG("R calc_base : %u\n", virtual_base);
  // clamp the virtual base to 4095
  virtual_base = MIN(4095, virtual_base);
  CDBG("R virtual_base : %u, input_slope : %f\n", virtual_base, ip->r_lut_delta[8]);
  // recalculate the new slope
  op->r_lut_delta[8] = CALC_SLOPE(op->r_lut_p[7], 4095,
    op->r_lut_base[8], virtual_base);
  CDBG("R virtual_base : %u, output_slope : %f\n", virtual_base, op->r_lut_delta[8]);

  // Gr channel
  virtual_base = ip->gr_lut_base[8] +
      (ip->gr_lut_delta[8]) * (4095 - ip->gr_lut_p[7]);
  CDBG("GR calc_base : %u\n", virtual_base);
  virtual_base = MIN(4095, virtual_base);
  CDBG("GR virtual_base : %u, input_slope : %f\n", virtual_base, ip->gr_lut_delta[8]);
  op->gr_lut_delta[8] = CALC_SLOPE(op->gr_lut_p[7], 4095,
    op->gr_lut_base[8], virtual_base);
  CDBG("GR virtual_base : %u, output_slope : %f\n", virtual_base, op->gr_lut_delta[8]);

  virtual_base = ip->gb_lut_base[8] +
      (ip->gb_lut_delta[8]) * (4095 - ip->gb_lut_p[7]);
  CDBG("GB calc_base : %u\n", virtual_base);
  virtual_base = MIN(4095, virtual_base);
  CDBG("GB virtual_base : %u, input_slope : %f\n", virtual_base, ip->gb_lut_delta[8]);
  op->gb_lut_delta[8] = CALC_SLOPE(op->gb_lut_p[7], 4095,
    op->gb_lut_base[8], virtual_base);
  CDBG("GB virtual_base : %u, output_slope : %f\n", virtual_base, op->gb_lut_delta[8]);

  virtual_base = ip->b_lut_base[8] +
      (ip->b_lut_delta[8]) * (4095 - ip->b_lut_p[7]);
  CDBG("B calc_base : %u\n", virtual_base);
  virtual_base = MIN(4095, virtual_base);
  CDBG("B virtual_base : %u, input_slope : %f\n", virtual_base, ip->b_lut_delta[8]);
  op->b_lut_delta[8] = CALC_SLOPE(op->b_lut_p[7], 4095,
    op->b_lut_base[8], virtual_base);
  CDBG("B virtual_base : %u, output_slope : %f\n", virtual_base, op->b_lut_delta[8]);

} /* isp_linear_update_lowlight_table */

/*===========================================================================
 * FUNCTION    - vfe_select_linear_table -
 *
 * DESCRIPTION:
 *==========================================================================*/
static void select_linear_table(isp_linear_mod_t *mod,
  isp_linear_lux_t lux, awb_cct_type cct, chromatix_linearization_type* output,
  isp_pix_trigger_update_input_t *trigger_params)
{
  int rc = 0;
  chromatix_linearization_type output1, output2;
  chromatix_parms_type *pchromatix =
    (chromatix_parms_type *)&trigger_params->cfg.chromatix_ptrs.chromatixPtr;
  chromatix_VFE_common_type *pchromatix_common =
    (chromatix_VFE_common_type *)
      trigger_params->cfg.chromatix_ptrs.chromatixComPtr;
  chromatix_black_level_type *pchromatix_black_level =
    &pchromatix->chromatix_VFE.chromatix_black_level;
  chromatix_L_type *pchromatix_L =
    &pchromatix_common->chromatix_L;
  trigger_ratio_t aec_rt;
  float awb_ratio = 0.0;
  uint16_t max_blk_increase;
  trigger_point_type blk_lowlight_trigger;
  int is_burst = IS_BURST_STREAMING(&trigger_params->cfg);

  aec_rt.ratio = 0.0;
  aec_rt.lighting = TRIGGER_NORMAL;

  max_blk_increase = pchromatix_black_level->max_blk_increase;
  blk_lowlight_trigger = pchromatix_black_level->blk_lowlight_trigger;

  rc =  isp_util_get_aec_ratio2(mod->notify_ops->parent,
                                pchromatix_L->control_linearization,
                                &(pchromatix_L->linearization_lowlight_trigger),
                                &blk_lowlight_trigger,
                                &(trigger_params->trigger_input.
                                  stats_update.aec_update),
                                is_burst,
                                &aec_rt);

  if (cct == AWB_CCT_TYPE_TL84_A)
    awb_ratio = GET_INTERPOLATION_RATIO(mod->trigger_info.mired_color_temp,
      mod->trigger_info.trigger_A.mired_start, mod->trigger_info.trigger_A.mired_end);
  else if (cct == AWB_CCT_TYPE_D65_TL84)
    awb_ratio = GET_INTERPOLATION_RATIO(mod->trigger_info.mired_color_temp,
      mod->trigger_info.trigger_d65.mired_end, mod->trigger_info.trigger_d65.mired_start);

  CDBG("%s: aec_ratio : %f :: awb_ratio : %f\n",__func__, aec_rt.ratio, awb_ratio);
  switch (lux) {
  /* Bright */
  case LINEAR_AEC_BRIGHT:
    switch(cct) {
    case AWB_CCT_TYPE_A:
      *output = pchromatix_L->linear_table_A_lowlight;
      break;
    case AWB_CCT_TYPE_D65:
      *output = pchromatix_L->linear_table_Day_lowlight;
      break;
    case AWB_CCT_TYPE_TL84_A:
      trigger_interpolate_linear_table(
        &(pchromatix_L->linear_table_TL84_lowlight),
        &(pchromatix_L->linear_table_A_lowlight), awb_ratio, output);
      break;
    case AWB_CCT_TYPE_D65_TL84:
      trigger_interpolate_linear_table(
        &(pchromatix_L->linear_table_Day_lowlight),
        &(pchromatix_L->linear_table_TL84_lowlight), awb_ratio, output);
      break;
    case AWB_CCT_TYPE_TL84:
    default:
      *output = pchromatix_L->linear_table_TL84_lowlight;
      break;
    }
  break;
  /* b/n Bright and Normal */
  case LINEAR_AEC_BRIGHT_NORMAL:
    switch(cct) {
    case AWB_CCT_TYPE_A:
      trigger_interpolate_linear_table(
        &(pchromatix_L->linear_table_A_normal),
        &(pchromatix_L->linear_table_A_lowlight), aec_rt.ratio, output);
      break;
    case AWB_CCT_TYPE_D65:
      trigger_interpolate_linear_table(
        &(pchromatix_L->linear_table_Day_normal),
        &(pchromatix_L->linear_table_Day_lowlight), aec_rt.ratio, output);
      break;
    case AWB_CCT_TYPE_TL84_A:
      /* Bilinear Interpolation */
      // Interpolate A bright and TL84 bright
      trigger_interpolate_linear_table(
        &(pchromatix_L->linear_table_TL84_lowlight),
        &(pchromatix_L->linear_table_A_lowlight), awb_ratio, &output1);
      // Interpolate A normal and TL84 normal
      trigger_interpolate_linear_table(
        &(pchromatix_L->linear_table_TL84_normal),
        &(pchromatix_L->linear_table_A_normal), awb_ratio, &output2);
      // Interpolate b/n the o/p1 and o/p2
      trigger_interpolate_linear_table(&(output2), &(output1),
        aec_rt.ratio, output);
      break;
    case AWB_CCT_TYPE_D65_TL84:
      // Interpolate TL84 bright and D65 bright
      trigger_interpolate_linear_table(
        &(pchromatix_L->linear_table_Day_lowlight),
        &(pchromatix_L->linear_table_TL84_lowlight), awb_ratio, &output1);
      // Interpolate TL84 normal and D65 normal
      trigger_interpolate_linear_table(
        &(pchromatix_L->linear_table_Day_normal),
        &(pchromatix_L->linear_table_TL84_normal), awb_ratio, &output2);
      // Interpolate
      trigger_interpolate_linear_table(&(output2), &(output1),
        aec_rt.ratio, output);
      break;
    case AWB_CCT_TYPE_TL84:
    default:
      trigger_interpolate_linear_table(
        &(pchromatix_L->linear_table_TL84_normal),
        &(pchromatix_L->linear_table_TL84_lowlight), aec_rt.ratio, output);
      break;
    }
  break;
    case LINEAR_AEC_NORMAL:
      switch(cct) {
      case AWB_CCT_TYPE_A:
        linear_update_lowlight_table(
          &(pchromatix_L->linear_table_A_normal), output, aec_rt.ratio,
          max_blk_increase);
        break;
      case AWB_CCT_TYPE_D65:
        linear_update_lowlight_table(
          &(pchromatix_L->linear_table_Day_normal), output, aec_rt.ratio,
          max_blk_increase);
          break;
      case AWB_CCT_TYPE_TL84_A:
        /* Bilinear Interpolation */
        // Interpolate TL84 normal and A normal
        trigger_interpolate_linear_table(
          &(pchromatix_L->linear_table_TL84_normal),
          &(pchromatix_L->linear_table_A_normal), awb_ratio, &output1);
        linear_update_lowlight_table(
          &(output1), output, aec_rt.ratio, max_blk_increase);
        break;
      case AWB_CCT_TYPE_D65_TL84:
        /* Bilinear Interpolation */
        // Interpolate A bright and TL84 bright
        trigger_interpolate_linear_table(
          &(pchromatix_L->linear_table_Day_normal),
          &(pchromatix_L->linear_table_TL84_normal), awb_ratio, &output1);
        linear_update_lowlight_table(&(output1), output,
          aec_rt.ratio, max_blk_increase);
        break;
      case AWB_CCT_TYPE_TL84:
      default:
        linear_update_lowlight_table(
          &(pchromatix_L->linear_table_TL84_normal), output, aec_rt.ratio,
          max_blk_increase);
        break;
    }
    mod->blk_inc_comp = (1.0 - aec_rt.ratio) * max_blk_increase;
  break;
    default:
      CDBG_HIGH("%s: Not Expected: lux : %d :: CCT : %d", __func__, lux, cct);
      break;
  }
} /* vfe_select_linear_table */

/*===========================================================================
 * FUNCTION      linearization_trigger_update
 *
 * DESCRIPTION
 *==========================================================================*/
static int linearization_trigger_update(isp_linear_mod_t *linear_mod,
  isp_pix_trigger_update_input_t *trigger_params, uint32_t in_param_size)
{
  int rc = 0;
  int is_burst = IS_BURST_STREAMING(&trigger_params->cfg);
  long bl_lux_index_end, bl_lux_index_start;
  long ll_lux_index_end, ll_lux_index_start;
  isp_linear_lux_t lux = LINEAR_AEC_NORMAL;
  awb_cct_type cct_type;
  chromatix_linearization_type output;
  uint8_t update_linear = FALSE;

  chromatix_parms_type *chroma3a =
     (chromatix_parms_type *)trigger_params->cfg.chromatix_ptrs.chromatixPtr;
  chromatix_black_level_type *pchromatix_black_level =
    &chroma3a->chromatix_VFE.chromatix_black_level;
  chromatix_VFE_common_type *pchromatix_common =
    (chromatix_VFE_common_type *)
      trigger_params->cfg.chromatix_ptrs.chromatixComPtr;
  chromatix_L_type *pchromatix_L =
    &pchromatix_common->chromatix_L;

  if (in_param_size != sizeof(isp_pix_trigger_update_input_t)) {
  /* size mismatch */
  CDBG_ERROR("%s: size mismatch, expecting = %d, received = %d",
         __func__, sizeof(isp_pix_trigger_update_input_t), in_param_size);
  return -1;
  }

  if (!linear_mod->linear_enable) {
    CDBG("%s: linearization not enabled", __func__);
    return 0;
  }

  if (!linear_mod->linear_trigger) {
    CDBG("%s: linearization tigger not enabled", __func__);
    return 0;
  }

  if (!isp_util_aec_check_settled(&(trigger_params->trigger_input.stats_update.aec_update))) {
    CDBG("%s: AEC not settled", __func__);
    return 0;
  }

  linear_mod->trigger_info.mired_color_temp =
    MIRED(trigger_params->trigger_input.stats_update.awb_update.color_temp);
  CALC_CCT_TRIGGER_MIRED(linear_mod->trigger_info.trigger_A,
    pchromatix_L->linear_A_trigger);
  CALC_CCT_TRIGGER_MIRED(linear_mod->trigger_info.trigger_d65,
    pchromatix_L->linear_D65_trigger);

  // get the cct type
  cct_type = isp_util_get_awb_cct_type(linear_mod->notify_ops->parent,
	                                     &linear_mod->trigger_info, chroma3a);

  // get the low light trigger points
  bl_lux_index_end = pchromatix_L->linearization_lowlight_trigger.lux_index_end;
  bl_lux_index_start = pchromatix_L->linearization_lowlight_trigger.lux_index_start;

  // get the low light trigger points
  ll_lux_index_end =
      pchromatix_black_level->blk_lowlight_trigger.lux_index_end;
  ll_lux_index_start =
      pchromatix_black_level->blk_lowlight_trigger.lux_index_start;

  /*============================================*/
  /* AEC :Decision */
  /*============================================*/
  float lux_idx = trigger_params->trigger_input.stats_update.aec_update.lux_idx;
  if (lux_idx <= bl_lux_index_end)
    lux = LINEAR_AEC_BRIGHT;
  else if (lux_idx > bl_lux_index_end  &&  lux_idx <= bl_lux_index_start)
    lux = LINEAR_AEC_BRIGHT_NORMAL;
  else if (lux_idx > bl_lux_index_start)
    lux = LINEAR_AEC_NORMAL;
  else
    CDBG_HIGH("%s: Lux index is invalid\n", __func__);

  /* check for trigger updation */
  update_linear = ((linear_mod->old_streaming_mode != trigger_params->cfg.streaming_mode) ||
    !F_EQUAL(linear_mod->prev_lux, lux) || !F_EQUAL(linear_mod->prev_cct_type, cct_type));

  if (update_linear) {
    select_linear_table(linear_mod, lux, cct_type, &output,
      trigger_params);
    config_linearization_cmd(linear_mod, &output);
    linear_mod->hw_update_pending = TRUE;
    CDBG("%s: color temp %d", __func__, trigger_params->trigger_input.stats_update.awb_update.color_temp);
    CDBG("%s: lux index %f", __func__, trigger_params->trigger_input.stats_update.aec_update.lux_idx);
    CDBG("%s: AEC type prev %s new %s", __func__, aec_debug_str[linear_mod->prev_lux],
      aec_debug_str[lux]);
    CDBG("%s: AWB type prev %s new %s", __func__, awb_debug_str[linear_mod->prev_cct_type],
      awb_debug_str[cct_type]);
    linear_mod->old_streaming_mode = trigger_params->cfg.streaming_mode;
    linear_mod->prev_lux = lux;
    linear_mod->prev_cct_type = cct_type;
  }
  return 0;

} /* linearization_trigger_update */

/*===========================================================================
 * FUNCTION    - linearization_init -
 *
 * DESCRIPTION:
 *==========================================================================*/
static int linearization_init (void *mod_ctrl, void *in_params, isp_notify_ops_t *notify_ops)
{
  isp_linear_mod_t *linear = mod_ctrl;
  isp_hw_mod_init_params_t *init_params = in_params;

  linear->fd = init_params->fd;
  linear->notify_ops = notify_ops;
  linear->old_streaming_mode = CAM_STREAMING_MODE_MAX;
  linear->hw_update_pending = FALSE;
  return 0;
}/* linearization_init */

/*===========================================================================
 * FUNCTION    - linearization_config -
 *
 * DESCRIPTION:
 *==========================================================================*/
static int linearization_config(isp_linear_mod_t *linear_mod, isp_hw_pix_setting_params_t *pix_settings,
                     uint32_t in_param_size)
{
  int  rc = 0;
  uint32_t i;
  chromatix_VFE_common_type *pchromatix_common =
    (chromatix_VFE_common_type *)pix_settings->chromatix_ptrs.chromatixComPtr;
  chromatix_L_type *pchromatix_L =
    &pchromatix_common->chromatix_L;

  CDBG("%s\n",__func__);

  if (in_param_size != sizeof(isp_hw_pix_setting_params_t)) {
  /* size mismatch */
  CDBG_ERROR("%s: size mismatch, expecting = %d, received = %d",
         __func__, sizeof(isp_hw_pix_setting_params_t), in_param_size);
  return -1;
  }

  config_linearization_cmd(linear_mod,
    &(pchromatix_L->linear_table_TL84_normal));

  linear_mod->hw_update_pending = TRUE;
  linear_mod->prev_cct_type = AWB_CCT_TYPE_TL84;
  linear_mod->prev_lux = LINEAR_AEC_NORMAL;
  linear_mod->old_streaming_mode = CAM_STREAMING_MODE_MAX;


  linear_mod->hw_update_pending = TRUE;

  return rc;
}

/* ============================================================
 * function name: linearization_enable
 * description: enable Linearization
 * ============================================================*/
static int linearization_enable(isp_linear_mod_t *linear,
                isp_mod_set_enable_t *enable,
                uint32_t in_param_size)
{
  if (in_param_size != sizeof(isp_mod_set_enable_t)) {
  /* size mismatch */
  CDBG_ERROR("%s: size mismatch, expecting = %d, received = %d",
         __func__, sizeof(isp_mod_set_enable_t), in_param_size);
  return -1;
  }
  linear->linear_enable = enable->enable;

  return 0;
}

/* ============================================================
 * function name: Linearization_trigger_enable
 * description: enable trigger update feature
 * ============================================================*/
static int linearization_trigger_enable(isp_linear_mod_t *linear,
                isp_mod_set_enable_t *enable,
                uint32_t in_param_size)
{
  if (in_param_size != sizeof(isp_mod_set_enable_t)) {
  /* size mismatch */
  CDBG_ERROR("%s: size mismatch, expecting = %d, received = %d",
         __func__, sizeof(isp_mod_set_enable_t), in_param_size);
  return -1;
  }
  linear->linear_trigger_enable = enable->enable;

  return 0;
}

/* ============================================================
 * function name: Linearization_destroy
 * description: close Linearization
 * ============================================================*/
static int linearization_destroy (void *mod_ctrl)
{
  isp_linear_mod_t *linear = mod_ctrl;

  memset(linear,  0,  sizeof(isp_linear_mod_t));
  free(linear);
  return 0;
}

/* ============================================================
 * function name: Linearization_set_params
 * description: set parameters
 * ============================================================*/
static int linearization_set_params (void *mod_ctrl, uint32_t param_id,
                     void *in_params, uint32_t in_param_size)
{
  isp_linear_mod_t *linear = mod_ctrl;
  int rc = 0;

  switch (param_id) {
  case ISP_HW_MOD_SET_MOD_ENABLE:
    rc = linearization_enable(linear, (isp_mod_set_enable_t *)in_params,
                     in_param_size);
    break;
  case ISP_HW_MOD_SET_MOD_CONFIG:
    rc = linearization_config(linear, (isp_hw_pix_setting_params_t *)in_params, in_param_size);
    break;
  case ISP_HW_MOD_SET_TRIGGER_ENABLE:
    rc = linearization_trigger_enable(linear, (isp_mod_set_enable_t *)in_params, in_param_size);
    break;
  case ISP_HW_MOD_SET_TRIGGER_UPDATE:
    rc = linearization_trigger_update(linear, (isp_pix_trigger_update_input_t *)in_params, in_param_size);
    break;
  default:
    CDBG_ERROR("%s: param_id is not supported in this module\n", __func__);
    break;
  }
  return rc;
}

/** linearization_ez_isp_update
 *  @linear_mod
 *  @linDiag
 *
 **/
static void linearization_ez_isp_update(
  isp_linear_mod_t *clf_module,
  linearization_t *linDiag)
{
  ISP_LinearizationLut *linCfg;
  int idx;
  linCfg = &(clf_module->applied_linear_lut);
  memcpy(linDiag, linCfg, sizeof(linearization_t));
}

/* ============================================================
 * function name: Linearization_get_params
 * description: get parameters
 * ============================================================*/
static int linearization_get_params (void *mod_ctrl, uint32_t param_id,
                     void *in_params, uint32_t in_param_size,
                     void *out_params, uint32_t out_param_size)
{
  int rc = 0;
  isp_linear_mod_t *linear = mod_ctrl;

  switch (param_id) {
  case ISP_HW_MOD_GET_MOD_ENABLE: {
    isp_mod_get_enable_t *enable = out_params;

    if (sizeof(isp_mod_get_enable_t) != out_param_size) {
      CDBG_ERROR("%s: error, out_param_size mismatch, param_id = %d",
                 __func__, param_id);
      break;
    }
    enable->enable = linear->linear_enable;
    break;
  }

  case ISP_HW_MOD_GET_VFE_DIAG_INFO_USER: {
    vfe_diagnostics_t *vfe_diag = (vfe_diagnostics_t *)out_params;
    linearization_t *linDiag;
    if (sizeof(vfe_diagnostics_t) != out_param_size) {
      CDBG_ERROR("%s: error, out_param_size mismatch, param_id = %d",
        __func__, param_id);
      break;
    }
    linDiag = &(vfe_diag->prev_linear);
    if(linear->old_streaming_mode == CAM_STREAMING_MODE_BURST) {
        linDiag = &(vfe_diag->snap_linear);
    }
    vfe_diag->control_linear.enable = linear->linear_enable;
    vfe_diag->control_linear.cntrlenable = linear->linear_trigger_enable;
    linearization_ez_isp_update(linear, linDiag);
    /*Populate vfe_diag data*/
    CDBG("%s: Populating vfe_diag data", __func__);
  }
    break;

  default:
    rc = -EPERM;
    break;
  }
  return rc;
}

/* ============================================================
 * function name: Linearization_do_hw_update
 * description: Linearization_do_hw_update
 * ============================================================*/
static int linearization_do_hw_update(isp_linear_mod_t *linear_mod)
{
    int i, rc = 0;
    struct msm_vfe_cfg_cmd2 cfg_cmd;
    struct msm_vfe_reg_cfg_cmd reg_cfg_cmd[7];
    uint32_t linear_lut_channel =
      (linear_mod->linear_cmd.CfgParams.lutBankSel == 0)?
        BLACK_LUT_RAM_BANK0 : BLACK_LUT_RAM_BANK1;

    if (linear_mod->hw_update_pending) {

      /* prepare dmi_set and dmi_reset fields */
      linear_mod->linear_cmd.dmi_set[0] =
        ISP32_DMI_CFG_DEFAULT + linear_lut_channel;
      linear_mod->linear_cmd.dmi_set[1] = 0;

      linear_mod->linear_cmd.dmi_reset[0] =
        ISP32_DMI_CFG_DEFAULT + ISP32_DMI_NO_MEM_SELECTED;
      linear_mod->linear_cmd.dmi_reset[1] = 0;

      cfg_cmd.cfg_data = (void *) &linear_mod->linear_cmd;
      cfg_cmd.cmd_len = sizeof(linear_mod->linear_cmd);
      cfg_cmd.cfg_cmd = (void *) reg_cfg_cmd;
      cfg_cmd.num_cfg = 7;

      /* set dmi to proper linearization bank */
      reg_cfg_cmd[0].u.rw_info.cmd_data_offset = 0;
      reg_cfg_cmd[0].cmd_type = VFE_WRITE_MB;
      reg_cfg_cmd[0].u.rw_info.reg_offset = ISP32_DMI_CFG_OFF;
      reg_cfg_cmd[0].u.rw_info.len = 1 * sizeof(uint32_t);

      reg_cfg_cmd[1].u.rw_info.cmd_data_offset =
        reg_cfg_cmd[0].u.rw_info.cmd_data_offset +
          reg_cfg_cmd[0].u.rw_info.len;
      reg_cfg_cmd[1].cmd_type = VFE_WRITE_MB;
      reg_cfg_cmd[1].u.rw_info.reg_offset = ISP32_DMI_ADDR;
      reg_cfg_cmd[1].u.rw_info.len = 1 * sizeof(uint32_t);

      /* send dmi data */
      reg_cfg_cmd[2].cmd_type = VFE_WRITE_DMI_32BIT;
      reg_cfg_cmd[2].u.dmi_info.hi_tbl_offset = 0;
      reg_cfg_cmd[2].u.dmi_info.lo_tbl_offset =
        reg_cfg_cmd[1].u.rw_info.cmd_data_offset +
          reg_cfg_cmd[1].u.rw_info.len;
      reg_cfg_cmd[2].u.dmi_info.len = sizeof(uint32_t) *
        ISP32_LINEARIZATON_TABLE_LENGTH;

      /* reset dmi to no bank*/
      reg_cfg_cmd[3].u.rw_info.cmd_data_offset =
        reg_cfg_cmd[2].u.dmi_info.lo_tbl_offset +
          reg_cfg_cmd[2].u.dmi_info.len;
      reg_cfg_cmd[3].cmd_type = VFE_WRITE_MB;
      reg_cfg_cmd[3].u.rw_info.reg_offset = ISP32_DMI_CFG_OFF;
      reg_cfg_cmd[3].u.rw_info.len = 1 * sizeof(uint32_t);

      reg_cfg_cmd[4].u.rw_info.cmd_data_offset =
        reg_cfg_cmd[3].u.rw_info.cmd_data_offset +
          reg_cfg_cmd[3].u.rw_info.len;
      reg_cfg_cmd[4].cmd_type = VFE_WRITE_MB;
      reg_cfg_cmd[4].u.rw_info.reg_offset = ISP32_DMI_ADDR;
      reg_cfg_cmd[4].u.rw_info.len = 1 * sizeof(uint32_t);

      /* linearization configuration */
      reg_cfg_cmd[5].u.rw_info.cmd_data_offset =
        reg_cfg_cmd[4].u.rw_info.cmd_data_offset +
          reg_cfg_cmd[4].u.rw_info.len;
      reg_cfg_cmd[5].cmd_type = VFE_WRITE;
      reg_cfg_cmd[5].u.rw_info.reg_offset = ISP_LINEARIZATION32_OFF0;
      reg_cfg_cmd[5].u.rw_info.len = ISP_LINEARIZATION32_LEN0 * sizeof(uint32_t);

            /* linearization configuration */
      reg_cfg_cmd[6].u.rw_info.cmd_data_offset =
        reg_cfg_cmd[5].u.rw_info.cmd_data_offset +
          reg_cfg_cmd[5].u.rw_info.len;
      reg_cfg_cmd[6].cmd_type = VFE_WRITE;
      reg_cfg_cmd[6].u.rw_info.reg_offset = ISP_LINEARIZATION32_OFF1;
      reg_cfg_cmd[6].u.rw_info.len = ISP_LINEARIZATION32_LEN1 * sizeof(uint32_t);

      print_isp_linearization_config(&linear_mod->linear_cmd);
      rc = ioctl(linear_mod->fd, VIDIOC_MSM_VFE_REG_CFG, &cfg_cmd);
      if (rc < 0){
        CDBG_ERROR("%s: HW update error, rc = %d", __func__, rc);
        return rc;
      }

      memcpy(&(linear_mod->applied_linear_lut),
        &(linear_mod->linear_lut), sizeof(ISP_LinearizationLut));
      linear_mod->linear_cmd.CfgParams.lutBankSel ^= 1;
      linear_mod->hw_update_pending = 0;
    }

    return rc;
}

/* ============================================================
 * function name: Linearization_action
 * description: processing the action
 * ============================================================*/
static int linearization_action (void *mod_ctrl, uint32_t action_code,
                 void *data, uint32_t data_size)
{
  int rc = 0;
  isp_linear_mod_t *linear = mod_ctrl;

  switch (action_code) {
  case ISP_HW_MOD_ACTION_HW_UPDATE:
    rc = linearization_do_hw_update(linear);
    break;
  default:
    /* no op */
    rc = -EAGAIN;
    CDBG_HIGH("%s: action code = %d is not supported. nop",
              __func__, action_code);
    break;
  }
  return rc;
}

/* ============================================================
 * function name: Linearization32_open
 * description: open Linearization
 * ============================================================*/
isp_ops_t *linearization32_open(uint32_t version)
{
  isp_linear_mod_t *linear = malloc(sizeof(isp_linear_mod_t));

  if (!linear) {
    /* no memory */
    CDBG_ERROR("%s: no mem",  __func__);
    return NULL;
  }
  memset(linear,  0,  sizeof(isp_linear_mod_t));
  linear->ops.ctrl = (void *)linear;
  linear->ops.init = linearization_init;
  /* destroy the module object */
  linear->ops.destroy = linearization_destroy;
  /* set parameter */
  linear->ops.set_params = linearization_set_params;
  /* get parameter */
  linear->ops.get_params = linearization_get_params;
  linear->ops.action = linearization_action;
  return &linear->ops;
}

