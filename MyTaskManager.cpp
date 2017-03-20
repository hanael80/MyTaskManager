// MyTaskManager.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MyTask.h"
#include <algorithm>
#include <vector>
#include <windows.h>


/// task map
typedef std::map< int, MyTaskPtr > TaskMap;

/// task map with tag
typedef std::map< std::string, TaskMap > TaskMapWithTag;

TaskMap g_taskMap;        ///< 태스크 맵
TaskMap g_activeTaskMap;  ///< 액티브 태스크 맵
TaskMap g_pendingTaskMap; ///< 지연된 태스크 맵

TaskMapWithTag g_taskListMapWithTag; ///< task map(key: tag, value: task list)

int g_createdTaskCount   = 0;
int g_completedTaskCount = 0;
int g_cancelledTaskCount = 0;
int g_actionCount        = 0;


void Print()
{
	system( "cls" );

	printf( "# Active Task List\n" );
	int no = 1;
	std::vector< MyTaskPtr > activeTaskList;
	activeTaskList.reserve( g_activeTaskMap.size() );
	for ( auto& pair : g_activeTaskMap )
		activeTaskList.push_back( pair.second );

	std::sort( activeTaskList.begin(), activeTaskList.end(), []( MyTaskPtr lhs, MyTaskPtr rhs )
	{
		return *lhs < *rhs;
	} );

	for ( MyTaskPtr task : activeTaskList )
		printf( "	%d. %s(#%d)\n", no++, task->name.c_str(), task->id );

	printf( "# Pending Task List\n" );

	std::vector< MyTaskPtr > pendingTaskList;
	pendingTaskList.reserve( g_pendingTaskMap.size() );
	for ( auto& pair : g_pendingTaskMap )
		pendingTaskList.push_back( pair.second );

	std::sort( pendingTaskList.begin(), pendingTaskList.end(), []( MyTaskPtr lhs, MyTaskPtr rhs )
	{
		return *lhs < *rhs;
	} );

	no = 1;
	for ( MyTaskPtr task : pendingTaskList )
		printf( "	%d. %s(#%d)\n", no++, task->name.c_str(), task->id );

	for ( auto& pair : g_taskListMapWithTag )
	{
		const std::string& tag     = pair.first;
		const TaskMap&     taskMap = pair.second;

		printf( "\n" );
		printf( "#Tag: %s\n", tag.c_str() );
		no = 1;
		for ( auto& pair2 : taskMap )
		{
			const MyTaskPtr& task = pair2.second;
			printf( "	%d. %s(#%d)\n", no++, task->name.c_str(), task->id );
		}
	}

	int remainingTaskCount = g_createdTaskCount - g_completedTaskCount - g_cancelledTaskCount;

	printf( "\n" );
	printf( "Action Count         : %d\n", g_actionCount        );
	printf( "Created Task Count   : %d\n", g_createdTaskCount   );
	printf( "Completed Task Count : %d\n", g_completedTaskCount );
	printf( "Cancelled Task Count : %d\n", g_cancelledTaskCount );
	printf( "Remaining Task Count : %d\n", remainingTaskCount   );
}

void PrintCode()
{
	FILE* file = fopen( "remaining_task.txt", "w" );
	for ( auto& pair : g_taskMap )
	{
		MyTaskPtr task = pair.second;
		fprintf( file, "create_task	%d	%s\n", task->id, task->name.c_str() );
		if ( task->priority )
			fprintf( file, "priority	%d	%d\n", task->id, task->priority );
	}

	for ( auto& pair : g_activeTaskMap )
	{
		MyTaskPtr task = pair.second;
		fprintf( file, "make_active	%d\n", task->id );
	}

	for ( auto& pair : g_taskListMapWithTag )
	{
		const std::string& tag      = pair.first;
		const TaskMap&     taskMap = pair.second;

		for ( auto& pair2 : taskMap )
		{
			const MyTaskPtr& task = pair2.second;
			fprintf( file, "tag	%d	%s\n", task->id, tag.c_str() );
		}
	}
	fclose( file );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	handles 'tag' command
///
/// @return	no returns
///////////////////////////////////////////////////////////////////////////////////////////////////
void HandleTag()
{
	int         id = atoi( strtok( nullptr, " \t" ) );
	std::string tag = strtok( nullptr, "\r\n" );

	auto iter = g_taskMap.find( id );
	if ( iter == g_taskMap.end() )
	{
		printf( "invalid task id: %d\n", id );
		fflush( stdin );
		getchar();
		exit( 1 );
	}

	MyTaskPtr task = iter->second;
	task->tagSet.insert( tag );
	g_taskListMapWithTag[ tag ][ task->id ] = task;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	deletes a task with id
///
/// @param	id	identifier
///
/// @return	no returns
///////////////////////////////////////////////////////////////////////////////////////////////////
void DeleteTask( int id )
{
	auto iter = g_taskMap.find( id );
	if ( iter == g_taskMap.end() )
	{
		printf( "invalid task. [id: %d]\n", id );
		return;
	}

	MyTaskPtr task = iter->second;
	for ( const std::string& tag : task->tagSet )
		g_taskListMapWithTag[ tag ].erase( id );

	g_activeTaskMap  .erase( id );
	g_pendingTaskMap .erase( id );
	g_taskMap        .erase( iter );
}

int main()
{
	FILE* file = fopen( "history.txt", "r" );
	char buf[ 512 ];
	while ( fgets( buf, sizeof( buf ) - 1, file ) )
	{
		char* command = strtok( buf, " \t\r\n" );
		if ( !command ) continue;

		if ( !strcmp( command, "create_task" ) || !strcmp( command, "create" ) )
		{
			int         id   = atoi( strtok( nullptr, " \t" ) );
			std::string name = strtok( nullptr, "\r\n" );

			auto iter = g_taskMap.find( id );
			if ( iter != g_taskMap.end() )
			{
				printf( "already existing id: %d\n", id );
				fflush( stdin );
				getchar();
				exit( 1 );
			}

			auto task = std::make_shared< MyTask >( id, name );
			g_taskMap[ id ] = task;
			g_pendingTaskMap[ id ] = task;

			g_createdTaskCount++;
		}
		else if ( !strcmp( command, "complete_task" ) || !strcmp( command, "complete" ) )
		{
			int id = atoi( strtok( nullptr, " \t\r\n" ) );

			DeleteTask( id );
			g_completedTaskCount++;
		}
		else if ( !strcmp( command, "cancel_task" ) || !strcmp( command, "cancel" ) )
		{
			int id = atoi( strtok( nullptr, " \t\r\n" ) );

			DeleteTask( id );
			g_cancelledTaskCount++;
		}
		else if ( !strcmp( command, "make_active" ) || !strcmp( command, "active" ) )
		{
			int id = atoi( strtok( nullptr, " \t\r\n" ) );

			MyTaskPtr task = g_taskMap[ id ];
			if ( !task )
			{
				printf( "invalid taskId: %d\n", id );
				fflush( stdin );
				getchar();
				exit( 1 );
			}

			g_pendingTaskMap.erase( id );
			g_activeTaskMap[ id ] = task;
		}
		else if ( !strcmp( command, "make_pending" ) || !strcmp( command, "pending" ) )
		{
			int id = atoi( strtok( nullptr, " \t\r\n" ) );

			auto task = g_taskMap[ id ];
			if ( !task )
			{
				printf( "invalid taskId: %d\n", id );
				fflush( stdin );
				getchar();
				exit( 1 );
			}

			g_activeTaskMap.erase( id );
			g_pendingTaskMap[ id ] = task;
		}
		else if ( !strcmp( command, "priority" ) )
		{
			int id    = atoi( strtok( nullptr, " \t" ) );
			int value = atoi( strtok( nullptr, "\r\n" ) );

			auto iter = g_taskMap.find( id );
			if ( iter == g_taskMap.end() )
			{
				printf( "invalid task id: %d\n", id );
				fflush( stdin );
				getchar();
				exit( 1 );
			}

			MyTaskPtr task = iter->second;
			task->priority = value;
		}
		else if ( !strcmp( command, "tag" ) )
		{
			HandleTag();
		}
		else
		{
			printf( "invalid action: %s\n", command );
			fflush( stdin );
			getchar();
			exit( 1 );
		}

		g_actionCount++;
		Print();
		Sleep( 20 );
	}

	Print();
	PrintCode();
	
	getchar();

	return 0;
}
