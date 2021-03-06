/*
 * Copyright (C) 2007-2009 Qualcomm Technologies, Inc. All rights reserved. Proprietary and Confidential.
 */

/*
 * Woodside Networks, Inc proprietary. All rights reserved.
 * This file aniAsfEvent.h is for the Event Manager (Function declarations)
 * Author:  U. Loganathan
 * Date:    May 16th 2002
 * History:-
 * Date     Modified by Modification Information
 *
 */

#ifndef _ANI_ASF_EVENT_H_
#define _ANI_ASF_EVENT_H_

typedef struct event tAniEvent;

typedef struct eventq tAniEventQ;

extern tAniEventQ *aniAsfEventQCreate(void (*) (long, void *));
extern void aniAsfEventQFree(tAniEventQ *);
extern void aniAsfEventCleanUp(tAniEventQ *);
extern int aniAsfEventSend(tAniEventQ *, long, void *);
extern int aniAsfEventProcess(tAniEventQ *);
extern void aniAsfEventStats(tAniEventQ *, char *);

#endif /* _ANI_ASF_EVENT_H_ */
