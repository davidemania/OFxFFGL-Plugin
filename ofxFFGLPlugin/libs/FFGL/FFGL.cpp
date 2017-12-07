////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FFGL.cpp
//
// FreeFrame is an open-source cross-platform real-time video effects plugin system.
// It provides a framework for developing video effects plugins and hosts on Windows, 
// Linux and Mac OSX. 
// 
// FreeFrameGL (FFGL) is an extension to the FreeFrame spec to support video processing
// with OpenGL on Windows, Linux, and Mac OSX.
//
// Copyright (c) 2002, 2003, 2004, 2006 www.freeframe.org
// All rights reserved. 
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Redistribution and use in source and binary forms, with or without modification, 
//	are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in
//    the documentation and/or other materials provided with the
//    distribution.
//  * Neither the name of FreeFrame nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
//	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//	IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
//	OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
//	OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
//	OF THE POSSIBILITY OF SUCH DAMAGE. 
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// First version, Marcus Clements (marcus@freeframe.org) 
// www.freeframe.org
//
// FreeFrame 1.0 upgrade by Russell Blakeborough
// email: boblists@brightonart.org
//
// FreeFrame 1.0 - 03 upgrade 
// and implementation of FreeFrame SDK methods by Gualtiero Volpe
// email: Gualtiero.Volpe@poste.it
//
// FFGL upgrade by Trey Harrison
// email: trey@harrisondigitalmedia.com
//
// Modified by Davide Manià for OpenFrameworks 080
// email: software@cogitamus.it
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Includes 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FFGLPluginSDK.h"
#include <memory.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static and extern variables used in the FreeFrame SDK 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern CFFGLPluginInfo* g_CurrPluginInfo;

//static CFreeFrameGLPlugin* s_pPrototype = NULL;
bool gLInstantiated = false;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FreeFrame SDK default implementation of the FreeFrame global functions. 
// Such function are called by the plugMain function, the only function a plugin exposes.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void *getInfo() 
{
	return (void *)(g_CurrPluginInfo->GetPluginInfo());
}

DWORD initialise(CFreeFrameGLPlugin* &instanceID)
{
    if (g_CurrPluginInfo==NULL)   // g_CurrPluginInfo is initialized in CFFGLPluginInfo constructor as "this"
    return FF_FAIL;             // so this is a check to see if a CFFGLPluginInfo has been instantiated
                                // CFFGLPluginInfo is created as a static object in main.cpp
                                // so it should always exist

    if (instanceID==NULL)
    {
        //get the instantiate function pointer (pointer to createPluginInstance)
        
        FPCREATEINSTANCEGL *pInstantiate = g_CurrPluginInfo->GetFactoryMethod();

        //call the instantiate function
        
        // *********************************
        // Here a new FFGLplugin is created!!!!!
        // *********************************
        
        DWORD dwRet = pInstantiate(&instanceID);

        //make sure the instantiate call worked
        if ((dwRet == FF_FAIL) || (instanceID == NULL))
          return FF_FAIL;

        return FF_SUCCESS;
    }

    return FF_SUCCESS; 
}

DWORD deInitialise(CFreeFrameGLPlugin* &instanceID)
{
	if (instanceID != NULL)
    {
		delete instanceID;
		instanceID = NULL;
	}
	return FF_SUCCESS;
}

DWORD getNumParameters(CFreeFrameGLPlugin* &instanceID)
{
	if (instanceID == NULL) {
		DWORD dwRet = initialise(instanceID);
		if (dwRet == FF_FAIL) return FF_FAIL;
	}

	return (DWORD) instanceID->GetNumParams();
}

// Bug in host?
// It reads parameters name in one of the many times it build a prototype, then it destroys it and rebuilds another
// one but it keeps the old address. At times (more or less at random) the pointer points to an invalid string,
// and displayed text is incorrect.

// Since parameter names will be the same in all the instances we keep them safe in a static object.
// Not the most beautiful solution but it works…

const int NAME_SIZE = 16;
class pname
{
public:
    pname()
    {
        for(int i=0; i < NAME_SIZE; i++)
            name[i]=0;
    }
    
    void setName(const char* source)
    {
        for(int i=0; i < NAME_SIZE; i++)
            name[i]=source[i];
    }
    
    char name[NAME_SIZE];
};

char* getParameterName(DWORD index, CFreeFrameGLPlugin* &instanceID)
{
	if (instanceID == NULL) {
		DWORD dwRet = initialise(instanceID);
		if (dwRet == FF_FAIL) return NULL;
	}
    
    static std::vector<pname> names;
    if(index+1 > names.size())
    {
        names.resize(index+1);
    }
    
    char* pname = instanceID->GetParamName(index);
    names[index].setName(pname);
    
	return names[index].name;
}

DWORD getParameterDefault(DWORD index, CFreeFrameGLPlugin* &instanceID)
{
	if (instanceID == NULL) {
		DWORD dwRet = initialise(instanceID);
		if (dwRet == FF_FAIL) return FF_FAIL;
	}

	void* pValue = instanceID->GetParamDefault(index);
	if (pValue == NULL) return FF_FAIL;
	else {
		if( instanceID->GetParamType(index) == FF_TYPE_TEXT ){
			return (DWORD)pValue;	// return address of string if text parameters, else we would be copying the first for bytes 
									// of the string into return value
		}else{
			DWORD dwRet;
			memcpy(&dwRet, pValue, 4);
			return dwRet;
		}
	}
}


DWORD getPluginCaps(DWORD index, CFreeFrameGLPlugin* &instanceID)
{
	int MinInputs = -1;
	int MaxInputs = -1;

	if (instanceID == NULL) {
		DWORD dwRet = initialise(instanceID);
		if (dwRet == FF_FAIL) return FF_FAIL;
	}

	switch (index) {

	case FF_CAP_16BITVIDEO:
		return FF_FALSE;

	case FF_CAP_24BITVIDEO:
		return FF_FALSE;

	case FF_CAP_32BITVIDEO:
		return FF_FALSE;

	case FF_CAP_PROCESSFRAMECOPY:
		return FF_FALSE;

  case FF_CAP_PROCESSOPENGL:
		return FF_TRUE;

  case FF_CAP_SETTIME:
    if (instanceID->GetTimeSupported())
      return FF_TRUE;
    else
      return FF_FALSE;

	case FF_CAP_MINIMUMINPUTFRAMES:
		MinInputs = instanceID->GetMinInputs();
		if (MinInputs < 0) return FF_FALSE;
		return DWORD(MinInputs);

	case FF_CAP_MAXIMUMINPUTFRAMES:
		MaxInputs = instanceID->GetMaxInputs();
		if (MaxInputs < 0) return FF_FALSE;
		return DWORD(MaxInputs);

	case FF_CAP_COPYORINPLACE:
  	return FF_FALSE;

	default:
		return FF_FALSE;
	}
	
	return FF_FAIL;
}

void *getExtendedInfo()
{
	return (void *)(g_CurrPluginInfo->GetPluginExtendedInfo());
}

DWORD getParameterType(DWORD index, CFreeFrameGLPlugin* &instanceID)
{
	if (instanceID == NULL) {
		DWORD dwRet = initialise(instanceID);
		if (dwRet == FF_FAIL) return FF_FAIL;
	}
	
	return instanceID->GetParamType(index);
}

DWORD instantiateGL(const FFGLViewportStruct *pGLViewport, CFreeFrameGLPlugin* &instanceID)
{
	if (g_CurrPluginInfo==NULL || pGLViewport==NULL)
    return FF_FAIL;

  // If the plugin is not initialized, initialize it
  if (instanceID == NULL)
  {
		DWORD dwRet = initialise(instanceID);

	  if ((dwRet == FF_FAIL) || (instanceID == NULL))
      return FF_FAIL;
	}
		
//	//get the instantiate function pointer
//  FPCREATEINSTANCEGL *pInstantiate = g_CurrPluginInfo->GetFactoryMethod();
//
//	CFreeFrameGLPlugin *pInstance = NULL;
//
//  //call the instantiate function
//    
//  // *********************************
//  // Here a new FFGLplugin is created!!!!!
//  // *********************************
//    
//  DWORD dwRet = pInstantiate(&pInstance);
//
//  //make sure the instantiate call worked
//  if ((dwRet == FF_FAIL) || (pInstance == NULL))
//    return FF_FAIL;
//
//	pInstance->m_pPlugin = pInstance;
		
	// Initializing instance with default values
	for (int i = 0; i < instanceID->GetNumParams(); ++i)
  {
		//DWORD dwType = s_pPrototype->GetParamType(DWORD(i));
		void* pValue = instanceID->GetParamDefault(DWORD(i));
		SetParameterStruct ParamStruct;
		ParamStruct.ParameterNumber = DWORD(i);
    
//#ifdef FFGLTEXTFIX
    if( instanceID->GetParamType(DWORD(i)) == FF_TYPE_TEXT )
    {
    	ParamStruct.NewParameterValue = (DWORD)pValue;
    }
    else
    {
			memcpy(&ParamStruct.NewParameterValue, pValue, 4);
    }
    /*
#else
  	memcpy(&ParamStruct.NewParameterValue, pValue, 4);
#endif
		*/
    
    DWORD dwRet = instanceID->SetParameter(&ParamStruct);
		if (dwRet == FF_FAIL)
        {
        //SetParameter failed, delete the instance
        delete instanceID;
        instanceID = NULL;
        return FF_FAIL;
        }
	}

	//call the InitGL method
  if (instanceID->InitGL(pGLViewport)==FF_SUCCESS)
  {
    //succes? we're done.
      gLInstantiated = true;
    return (DWORD)instanceID;
  }

  //InitGL failed, delete the instance
  instanceID->DeInitGL();
  delete instanceID;
    instanceID = NULL;
  return FF_FAIL;
}

DWORD deInstantiateGL(CFreeFrameGLPlugin* &instanceID)
{
  //CFreeFrameGLPlugin *p = (CFreeFrameGLPlugin *)instanceID;

	if (instanceID != NULL)
  {
    instanceID->DeInitGL();
		delete instanceID;
      instanceID = NULL;
		return FF_SUCCESS;
	}

	return FF_FAIL;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of plugMain, the one and only exposed function
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32

   plugMainUnion __stdcall plugMain(DWORD functionCode, DWORD inputValue, DWORD instanceID) 

#elif TARGET_OS_MAC

   plugMainUnion plugMain(DWORD functionCode, DWORD inputValue, DWORD instanceID) 

#elif __linux__

   plugMainUnion plugMain(DWORD functionCode, DWORD inputValue, DWORD instanceID)

#endif	

{
	plugMainUnion retval;

	// declare pPlugObj - pointer to this instance
	CFreeFrameGLPlugin* pPlugObj;

	// typecast DWORD into pointer to a CFreeFrameGLPlugin
	pPlugObj = (CFreeFrameGLPlugin*) instanceID;
    gLInstantiated = false;
    
	switch (functionCode) {

	case FF_GETINFO:
		retval.PISvalue = (PluginInfoStruct*)getInfo();
		break;

	case FF_INITIALISE:
		retval.ivalue = initialise(pPlugObj);
		break;

	case FF_DEINITIALISE:
		retval.ivalue = deInitialise(pPlugObj);
		break;

	case FF_GETNUMPARAMETERS:
		retval.ivalue = getNumParameters(pPlugObj);
		break;

	case FF_GETPARAMETERNAME:
		retval.svalue = getParameterName(inputValue, pPlugObj);
		break;
	
	case FF_GETPARAMETERDEFAULT:
		retval.ivalue = getParameterDefault(inputValue, pPlugObj);
		break;

	case FF_GETPLUGINCAPS:
		retval.ivalue = getPluginCaps(inputValue, pPlugObj);
		break;

	case FF_GETEXTENDEDINFO: 
		retval.ivalue = (DWORD) getExtendedInfo();
		break;

	case FF_GETPARAMETERTYPE:		
		retval.ivalue = getParameterType(inputValue, pPlugObj);
		break;

	case FF_GETPARAMETERDISPLAY:
		if (pPlugObj != NULL) 
			retval.svalue = pPlugObj->GetParameterDisplay(inputValue);
		else
			retval.svalue = (char*)FF_FAIL;
		break;
		
	case FF_SETPARAMETER:
		if (pPlugObj != NULL)
			retval.ivalue = pPlugObj->SetParameter((const SetParameterStruct*) inputValue);
		else
			retval.ivalue = FF_FAIL;
		break;
	
	case FF_GETPARAMETER:
		if (pPlugObj != NULL) 
			retval.ivalue = pPlugObj->GetParameter(inputValue);
		else 
			retval.ivalue = FF_FAIL;
		break;
		
  case FF_INSTANTIATEGL:
    // Only here the address of the plugin will be passed to host. Others are lost (and must be destroyed)
    retval.ivalue = (DWORD)instantiateGL((const FFGLViewportStruct *)inputValue, pPlugObj);
    break;

  case FF_DEINSTANTIATEGL:
    if (pPlugObj != NULL)
			retval.ivalue = deInstantiateGL(pPlugObj);
		else
			retval.ivalue = FF_FAIL;
    break;
	
	case FF_GETIPUTSTATUS:
		if (pPlugObj != NULL)
			retval.ivalue = pPlugObj->GetInputStatus(inputValue);
		else
			retval.ivalue = FF_FAIL;
		break;

  case FF_PROCESSOPENGL:
    if (pPlugObj != NULL)
    {
      ProcessOpenGLStruct *pogls = (ProcessOpenGLStruct *)inputValue;
      if (pogls!=NULL)
        retval.ivalue = pPlugObj->ProcessOpenGL(pogls);
      else
        retval.ivalue = FF_FAIL;
    }
		else
			retval.ivalue = FF_FAIL;
		break;

  case FF_SETTIME:
    if (pPlugObj != NULL)
    {
      double *inputTime = (double *)inputValue;
      if (inputTime!=NULL)
        retval.ivalue = pPlugObj->SetTime(*inputTime);
      else
        retval.ivalue = FF_FAIL;
    }
		else
			retval.ivalue = FF_FAIL;
		break;

  //these old FF functions must always fail for FFGL plugins
	case FF_INSTANTIATE:
  case FF_DEINSTANTIATE:
	case FF_PROCESSFRAME:
  case FF_PROCESSFRAMECOPY:
	default:
		retval.ivalue = FF_FAIL;
		break;
	}
	
    if(instanceID == 0 && pPlugObj != 0 && !gLInstantiated)
    {
        deInitialise(pPlugObj);
    }
	return retval;
}
