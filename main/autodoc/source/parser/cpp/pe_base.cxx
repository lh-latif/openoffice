/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/

#include <precomp.h>
#include "pe_base.hxx"


// NOT FULLY DECLARED SERVICES
#include <cosv/tpl/tpltools.hxx>
#include <ary/cpp/c_gate.hxx>
#include <ary/cpp/c_type.hxx>
#include "pe_type.hxx"




namespace cpp
{


static const PE_Base::Base aNullBase_;


PE_Base::PE_Base( Cpp_PE * i_pParent )
	:   Cpp_PE(i_pParent),
		pStati(new PeStatusArray<PE_Base>)
		// aBaseIds,
		// pSpType,
		// pSpuBaseName
{
	Setup_StatusFunctions();
	aBaseIds.reserve(4);

	pSpType     	= new SP_Type(*this);
	pSpuBaseName	= new SPU_BaseName(*pSpType, 0, &PE_Base::SpReturn_BaseName);
}


PE_Base::~PE_Base()
{
}

void
PE_Base::Call_Handler( const cpp::Token & i_rTok )
{
	pStati->Cur().Call_Handler(i_rTok.TypeId(), i_rTok.Text());
}

void
PE_Base::Setup_StatusFunctions()
{
	typedef CallFunction<PE_Base>::F_Tok	F_Tok;
	static F_Tok stateF_startOfNext[] =	  	{ &PE_Base::On_startOfNext_Identifier,
											  &PE_Base::On_startOfNext_public,
											  &PE_Base::On_startOfNext_protected,
											  &PE_Base::On_startOfNext_private,
											  &PE_Base::On_startOfNext_virtual,
											  &PE_Base::On_startOfNext_DoubleColon };
	static INT16 stateT_startOfNext[] =    	{ Tid_Identifier,
											  Tid_public,
											  Tid_protected,
											  Tid_private,
											  Tid_virtual,
											  Tid_DoubleColon };
	static F_Tok stateF_inName[] = 			{ &PE_Base::On_inName_Identifier,
											  &PE_Base::On_inName_virtual,
											  &PE_Base::On_inName_SwBracket_Left,
											  &PE_Base::On_inName_DoubleColon,
											  &PE_Base::On_inName_Comma };
	static INT16 stateT_inName[] = 			{ Tid_Identifier,
											  Tid_virtual,
											  Tid_SwBracket_Left,
											  Tid_DoubleColon,
											  Tid_Comma };

	SEMPARSE_CREATE_STATUS(PE_Base, startOfNext, Hdl_SyntaxError);
	SEMPARSE_CREATE_STATUS(PE_Base, inName, Hdl_SyntaxError);
}

void
PE_Base::Hdl_SyntaxError( const char * i_sText)
{
    StdHandlingOfSyntaxError(i_sText);
}

void
PE_Base::InitData()
{
	pStati->SetCur(startOfNext);
	csv::erase_container(aBaseIds);
	aBaseIds.push_back(aNullBase_);
}

void
PE_Base::TransferData()
{
	// Does nothing.
}

void
PE_Base::SpReturn_BaseName()
{
	CurObject().nId = pSpuBaseName->Child().Result_Type().Id();

	static StreamStr aBaseName(100);
	aBaseName.seekp(0);
	pSpuBaseName->Child().Result_Type().Get_Text( aBaseName, Env().AryGate() );

	Env().Event_Class_FinishedBase(aBaseName.c_str());
}

void
PE_Base::On_startOfNext_public(const char *)
{
	SetTokenResult(done, stay);
	pStati->SetCur(inName);

	CurObject().eProtection = ary::cpp::PROTECT_public;
}

void
PE_Base::On_startOfNext_protected(const char *)
{
	SetTokenResult(done, stay);
	pStati->SetCur(inName);

	CurObject().eProtection = ary::cpp::PROTECT_protected;
}

void
PE_Base::On_startOfNext_private(const char *)
{
	SetTokenResult(done, stay);
	pStati->SetCur(inName);

	CurObject().eProtection = ary::cpp::PROTECT_private;
}

void
PE_Base::On_startOfNext_virtual(const char *)
{
	SetTokenResult(done, stay);

	CurObject().eVirtuality = ary::cpp::VIRTUAL_virtual;
}

void
PE_Base::On_startOfNext_Identifier(const char * )
{
	pSpuBaseName->Push(not_done);
}

void
PE_Base::On_startOfNext_DoubleColon(const char *)
{
	pSpuBaseName->Push(not_done);
}

void
PE_Base::On_inName_Identifier(const char * )
{
	pSpuBaseName->Push(not_done);
}

void
PE_Base::On_inName_virtual(const char *)
{
	SetTokenResult(done, stay);

	CurObject().eVirtuality = ary::cpp::VIRTUAL_virtual;
}

void
PE_Base::On_inName_DoubleColon(const char *)
{
	pSpuBaseName->Push(not_done);
}

void
PE_Base::On_inName_Comma(const char *)
{
	SetTokenResult(done, stay);
	pStati->SetCur(startOfNext);

	aBaseIds.push_back( aNullBase_ );
}

void
PE_Base::On_inName_SwBracket_Left(const char *)
{
	SetTokenResult(not_done, pop_success);
}


}   // namespace cpp





