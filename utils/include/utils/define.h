#ifndef DEFINE_H
#define DEFINE_H

/* Error codes */

#define MC_ERR_NOERROR                  0x00000000

/* General errors */

#define MC_ERR_ALLOCATION               0x00000001
#define MC_ERR_TOO_FEW_PARAMETERS       0x00000002
#define MC_ERR_MISSING_PARAMETER        0x00000003
#define MC_ERR_OPERATION_NOT_SUPPORTED  0x00000004
#define MC_ERR_INVALID_PARAMETER_VALUE  0x00000005
#define MC_ERR_INTERNAL_ERROR           0x00000006
#define MC_ERR_FILE_READ_ERROR          0x00000007
#define MC_ERR_FILE_WRITE_ERROR         0x00000008
#define MC_ERR_CONNECTION_ERROR         0x00000009
#define MC_ERR_DBOPEN_ERROR             0x0000000A
#define MC_ERR_CORRUPTED                0x0000000B
#define MC_ERR_NOT_ALLOWED              0x0000000C
#define MC_ERR_WRONG_SCRIPT             0x0000000D
#define MC_ERR_FOUND                    0x0000000E
#define MC_ERR_NOT_FOUND                0x0000000F
#define MC_ERR_NOT_SUPPORTED            0x00000010
#define MC_ERR_ERROR_IN_SCRIPT          0x00000011


#define MC_FOM_NONE                     0x00000000
#define MC_FOM_RELATIVE_TO_DATADIR      0x00000001
#define MC_FOM_RELATIVE_MASK            0x0000000F
#define MC_FOM_CREATE_DIR               0x00000100

#define MC_BUF_MODE_DEFAULT             0x00000000
#define MC_BUF_MODE_MAP                 0x00000001


#define MC_PRM_NETWORK_NAME_MAX_SIZE    32

#endif // DEFINE_H
