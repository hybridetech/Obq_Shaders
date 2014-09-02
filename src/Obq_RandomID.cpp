/*
Obq_RandomID :

random int based on object name.

*------------------------------------------------------------------------
Copyright (c) 2012-2014 Marc-Antoine Desjardins, ObliqueFX (madesjardins@obliquefx.com)

Permission is hereby granted, free of charge, to any person obtaining a copy 
of this software and associated documentation files (the "Software"), to deal 
in the Software without restriction, including without limitation the rights 
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.

Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php
*------------------------------------------------------------------------
*/

#include "Obq_Common.h"



// Arnold Thingy
//
AI_SHADER_NODE_EXPORT_METHODS(ObqRandomIDSimpleMethods);

// enum of parameters
//
enum ObqRandomIDSimpleParams {p_randMax, p_seed, p_stripModelName, p_stripFrameNumber, p_stripInstanceFrameNumber, p_stripInstanceID,p_stripInstanceShape};

// parameters
//
node_parameters
{
	AiParameterINT("randMax", 4);	
	AiParameterINT("seed", 233);
	AiParameterBOOL("stripModelName", true);
	AiParameterBOOL("stripFrameNumber", true);
	AiParameterBOOL("stripInstanceFrameNumber", true);
	AiParameterBOOL("stripInstanceID", false);
	AiParameterBOOL("stripInstanceShape", false);
}

node_initialize
{

}

node_update
{

}


node_finish
{

}

shader_evaluate
{
	// GET NAME (This is far from optimal)
	std::string name(AiNodeGetName(sg->Op));
	
	// TEST .SItoA.
	std::size_t len = name.length();
	std::size_t lastSItoA = name.rfind(".SItoA.");
	std::size_t startNameIndex = 0;
	std::size_t endNameIndex = lastSItoA;

	if(endNameIndex==std::string::npos)
	{
		AiMsgError("[Obq_RandomID] : Couldn't find the frame number separator. Name is : %s and separator is .SItoA.",name.c_str());
		sg->out.RGBA = AI_RGBA_BLACK;
		return; 
	}

	// INSTANCE TEST
	std::size_t firstSItoA = name.find(".SItoA.");

	// STRIP MODEL
	if(AiShaderEvalParamBool(p_stripModelName))
	{
		
		std::size_t startDotModel = name.find('.');
		if(startDotModel < firstSItoA)
			startNameIndex = startDotModel+1;
	}

	// NOT STRIP FRAME#
	if(!AiShaderEvalParamBool(p_stripFrameNumber) || endNameIndex >= len)
		endNameIndex = len;

	// INSTANCE
	bool stripInstanceFrameNumber = AiShaderEvalParamBool(p_stripInstanceFrameNumber);
	bool stripInstanceID = AiShaderEvalParamBool(p_stripInstanceID);
	bool stripInstanceShape = AiShaderEvalParamBool(p_stripInstanceShape);

	if(stripInstanceFrameNumber || stripInstanceID || stripInstanceShape)
	{
		std::size_t startInstanceSItoA = name.find(".SItoA.Instance",firstSItoA);
		if(startInstanceSItoA != std::string::npos)
		{
			// strip the SItoA.Instance
			std::size_t startInstanceFrameNumber = startInstanceSItoA + 15;
			std::size_t startInstanceID = name.find(".",startInstanceFrameNumber+1);
			std::size_t startInstanceShape = name.find(" ",startInstanceID);

			std::string instanceName("");

			if(!stripInstanceFrameNumber)
				instanceName += name.substr(startInstanceFrameNumber,startInstanceID-startInstanceFrameNumber);

			if(!stripInstanceID)
				instanceName += name.substr(startInstanceID,startInstanceShape-startInstanceID);

			if(!stripInstanceShape)
				instanceName += name.substr(startInstanceShape,lastSItoA-startInstanceShape);

			name = name.substr(0,startInstanceSItoA) + instanceName + name.substr(lastSItoA);
			endNameIndex -= ((lastSItoA-startInstanceSItoA) - instanceName.length());
		}
	}

	// Add character
	unsigned int a = AiShaderEvalParamInt(p_seed);

	for(std::size_t i = startNameIndex; i < endNameIndex; i++)
		//a += unsigned int(name[i]*name[i]*name[i])%31;
		a += (unsigned int)(name[i]*name[i])%73;
		//a += unsigned int(name[i]);

	// thanks to Mike F : http://stackoverflow.com/questions/167735/fast-pseudo-random-number-generator-for-procedural-content
    a = (a ^ 61) ^ (a >> 16);
    a = a + (a << 3);
    a = a ^ (a >> 4);
    a = a * 0x27d4eb2d;
    a = a ^ (a >> 15);

	sg->out.INT = static_cast<int>(a%AiShaderEvalParamInt(p_randMax));

}

//node_loader
//{
//	if (i > 0)
//		return FALSE;
//
//	node->methods      = ObqRandomIDSimpleMethods;
//	node->output_type  = AI_TYPE_INT;
//	node->name         = "Obq_RandomID";
//	node->node_type    = AI_NODE_SHADER;
//	strcpy(node->version, AI_VERSION);
//	return TRUE;
//}
