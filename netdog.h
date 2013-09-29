/*############################################################################
#                                                                            #
#  MODULE:     NETDOG.H                                                      #
#                                                                            #
#  Copyright (c) 2000 Rainbow Goldensoft Co., Ltd.  All Rights Reserved.     #
#                                                                            #
#  Description:  Netdog (TD-NF) client Win32 API Header file.                #
#                                                                            #
############################################################################*/

#ifndef _NETDOG_
#define _NETDOG_

#ifdef __cplusplus

extern "C" short int NetDogAddr,NetDogBytes;
extern "C" unsigned long NetDogPassword;
extern "C" unsigned long NetDogResult;
extern "C" void FAR * NetDogData;
extern "C" unsigned long FAR * NetDogHandle;

extern "C" {

#endif /* __cplusplus */

extern DWORD NetDogLogin (void);       /* login into network dog server */
extern DWORD NetDogLogout (void);      /* logout from network dog server */
extern DWORD NetDogRead (void);        /* invoke read dog service */
extern DWORD NetDogConvert (void);     /* invoke convert service */

#define DOGSUCCESS 0

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _NETDOG_ */
