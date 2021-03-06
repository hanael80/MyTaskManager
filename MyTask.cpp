#include "stdafx.h"
#include "MyTask.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	생성자
///
/// @param	id  	식별자
/// @param	name	이름
////////////////////////////////////////////////////////////////////////////////////////////////////
MyTask::MyTask( int id, const std::string& name )
:
id       ( id   ),
name     ( name ),
priority ( 0    )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	< 연산자를 재정의한다.
/// 
/// @param	rhs	대상 객체
/// 
/// @return	
////////////////////////////////////////////////////////////////////////////////////////////////////
bool MyTask::operator < ( const MyTask& rhs ) const
{
	if ( priority > rhs.priority ) return true;
	if ( priority < rhs.priority ) return false;

	return id > rhs.id;
}
