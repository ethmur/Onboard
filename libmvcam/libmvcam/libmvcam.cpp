// libmvcam.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "libmvcam.h"

/*
// This is an example of an exported variable
LIBMVCAM_API int nlibmvcam=0;

// This is an example of an exported function.
LIBMVCAM_API int fnlibmvcam(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see libmvcam.h for the class definition
Clibmvcam::Clibmvcam()
{
    return;
}*/


#include "stdafx.h"
#include "libmvcam.h"



#ifdef _M_X64
#pragma comment(lib, "../DVPCamera/DVPCamera64.lib")
#else
#pragma comment(lib, "../DVPCamera/DVPCamera.lib")
#endif




static mvStatus isValidHandle(dvpHandle *handle, dvpStatus *ret_stat) {
	if (handle == NULL || ret_stat == NULL) {
		return MV_INVAL_ERROR;
	}

	bool bValidHandle;
	dvpStatus status = dvpIsValid(*handle, &bValidHandle);
	if (status != DVP_STATUS_OK) {
		*ret_stat = status;
		return MV_DVP_ERROR;
	}
	return MV_OK;

}

 mvStatus mvCamOpen(char *camName, dvpHandle *handle, dvpStatus *ret_stat) {
	if (camName == NULL || camName == "" || ret_stat == NULL) {
		return MV_INVAL_ERROR;
	}

	dvpStatus status;
	status = dvpOpenByName(camName, OPEN_NORMAL, handle);

	if (status != DVP_STATUS_OK) {
		*ret_stat = status;
		return MV_DVP_ERROR;
	}

	return isValidHandle(handle, ret_stat);
}

 mvStatus mvCamScan(char *output, int size, dvpStatus *ret_stat) {

	if (output == NULL || size <= 0 || ret_stat == NULL) {
		return MV_INVAL_ERROR;
	}

	dvpStatus status;
	dvpUint32 i, n = 0;
	dvpCameraInfo info[16];

	status = dvpRefresh(&n);
	if (status != DVP_STATUS_OK) {
		*ret_stat = status;
		return MV_DVP_ERROR;
	}

	else if (status == DVP_STATUS_OK) {

		if (n > 16)
			n = 16;
		for (i = 0; i < n; i++) {
			status = dvpEnum(i, &info[i]);
			int len;
			if (status == DVP_STATUS_OK && (len = strlen(info[i].FriendlyName) + 1) <= size) {
				strcpy(output, info[i].FriendlyName);
				return MV_OK;
			}
			else if (status == DVP_STATUS_OK && len > size) {
				return MV_NOMEM_ERROR;
			}
			else {
				*ret_stat = status;
				return MV_DVP_ERROR;
			}
		}




	};

	return MV_NOCAM_ERROR;


}


mvStatus mvCamOpenDef(dvpHandle *handle, dvpStatus *ret_stat) {
	mvStatus status;
	char camera_out[MAX_CAMERA_NAME + 1];
	if ((status = mvCamScan(camera_out, MAX_CAMERA_NAME, ret_stat)) != MV_OK) {
		return status;
	}

	return mvCamOpen(camera_out, handle, ret_stat);

}


mvStatus mvCamSetExposure(dvpHandle *handle, mvExposure exp, dvpStatus *ret_stat) {
	dvpStatus status;
	/*
	if ((status = dvpSetAeMode(*handle, exp.mvexp_aemode)) != DVP_STATUS_OK) {
	*ret_stat = status;
	return MV_DVP_ERROR;
	}

	if ((status = dvpSetAeOperation(*handle, exp.mvexp_aeop)) != DVP_STATUS_OK) {
	*ret_stat = status;
	return MV_DVP_ERROR;
	}

	if (exp.mvexp_aetarget != 0) {
	if ((status = dvpSetAeTarget(*handle, exp.mvexp_aetarget)) != DVP_STATUS_OK) {
	*ret_stat = status;
	return MV_DVP_ERROR;
	}
	}

	*/

	if ((status = dvpSetAnalogGain(*handle, exp.mvexp_gain)) != DVP_STATUS_OK) {
		*ret_stat = status;
		return MV_DVP_ERROR;
	}


	if ((status == dvpSetAwbOperation(*handle, exp.mvexp_awop)) != DVP_STATUS_OK) {
		*ret_stat = status;
		return MV_DVP_ERROR;
	}


	if ((status = dvpSetAntiFlick(*handle, exp.mvexp_aflick)) != DVP_STATUS_OK) {
		*ret_stat = status;
		return MV_DVP_ERROR;
	}

	if ((status = dvpSetExposure(*handle, exp.mvexp_shutter)) != DVP_STATUS_OK) {
		*ret_stat = status;
		return MV_DVP_ERROR;
	}




	return MV_OK;
}

mvStatus mvCamDestroy(dvpHandle *handle, dvpStatus *ret_stat) {
	if (handle == NULL || ret_stat == NULL) {
		return MV_INVAL_ERROR;
	}
	dvpStatus status;
	mvStatus  m_stat;
	if ((m_stat = isValidHandle(handle, ret_stat)) != MV_OK) {
		return m_stat;
	}

	status = dvpClose(*handle);
	if (status != DVP_STATUS_OK) {
		*ret_stat = status;
		return MV_DVP_ERROR;
	}

	return MV_OK;


}

static mvStatus mvCamSetTriggerDelay(dvpHandle *handle, double trigger_delay, dvpStatus *ret_stat) {
	if (handle == NULL || ret_stat == NULL) {
		return MV_INVAL_ERROR;
	}

	if (isValidHandle(handle, ret_stat) != MV_OK) {
		return MV_DVP_ERROR;
	}

	dvpStatus status = dvpSetTriggerDelay(*handle, trigger_delay);
	if (status != DVP_STATUS_OK) {
		*ret_stat = status;
		return MV_DVP_ERROR;
	}

	return MV_OK;
}

static mvStatus mvCamStartTriggerLoop(dvpHandle *handle, double trigger_loop, dvpStatus *ret_stat) {

	if (handle == NULL || ret_stat == NULL) {
		return MV_INVAL_ERROR;
	}

	if (isValidHandle(handle, ret_stat) != MV_OK) {
		return MV_DVP_ERROR;
	}



	dvpStatus status = dvpSetSoftTriggerLoop(*handle, trigger_loop);
	if (status != DVP_STATUS_OK) {
		*ret_stat = status;
		return MV_DVP_ERROR;
	}

	if ((status = dvpSetSoftTriggerLoopState(*handle, TRUE)) != DVP_STATUS_OK) {
		*ret_stat = status;
		return MV_DVP_ERROR;
	}

	if ((status = dvpSetTriggerState(*handle, TRUE)) != DVP_STATUS_OK) {
		*ret_stat = status;
		return MV_DVP_ERROR;
	}

	if ((status = dvpStart(*handle)) != DVP_STATUS_OK) {
		*ret_stat = status;
		return MV_DVP_ERROR;
	}


	return MV_OK;
}


mvStatus mvCamStartTrigger(dvpHandle *handle, double delay, double loop, dvpStatus *ret_stat) {
	mvStatus status;
	if ((status = mvCamSetTriggerDelay(handle, delay, ret_stat)) != MV_OK) {
		return status;
	}

	return mvCamStartTriggerLoop(handle, loop, ret_stat);
}

mvStatus mvCamStopTrigger(dvpHandle *handle, dvpStatus *ret_stat) {
	if (handle == NULL || ret_stat == NULL) {
		return MV_INVAL_ERROR;
	}

	if (isValidHandle(handle, ret_stat) != MV_OK) {
		return MV_DVP_ERROR;
	}

	dvpStatus status;
	if ((status = dvpSetSoftTriggerLoopState(*handle, FALSE)) != DVP_STATUS_OK) {
		*ret_stat = status;
		return MV_DVP_ERROR;
	}

	if ((status = dvpStop(*handle)) != DVP_STATUS_OK) {
		*ret_stat = status;
		return MV_DVP_ERROR;
	}

	return MV_OK;
}


mvStatus mvCamGetImage(dvpHandle *handle, mvCamImage *image, dvpUint32 timeout, dvpStatus *ret_stat) {
	dvpStatus status;

	if (handle == NULL || ret_stat == NULL) {
		return MV_INVAL_ERROR;
	}

	if (isValidHandle(handle, ret_stat) != MV_OK) {
		return MV_DVP_ERROR;
	}

	status = dvpGetFrame(*handle, &image->frame, &image->image_buffer, timeout);

	if (status != DVP_STATUS_OK) {
		*ret_stat = status;
		return MV_DVP_ERROR;
	}

	return MV_OK;
}

mvStatus mvCamSaveImage(dvpHandle *handle, mvCamImage *image, int quality, dvpStatus *ret_stat) {
	if (handle == NULL || ret_stat == NULL || image == NULL) {
		return MV_INVAL_ERROR;
	}

	if (isValidHandle(handle, ret_stat) != MV_OK) {
		return MV_DVP_ERROR;
	}
	dvpStatus status = dvpSavePicture(&image->frame, image->image_buffer, image->image_name, quality);
	if (status != DVP_STATUS_OK) {
		*ret_stat = status;
		return MV_DVP_ERROR;
	}

	return MV_OK;

}
