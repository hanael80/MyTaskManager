#pragma once


class MyTask
{
public:
	int         id;       ///< 식별자
	std::string name;     ///< 이름
	int         priority; ///< 우선 순위

public:
	/// 생성자
	MyTask( int id, const std::string& name );

	/// < 연산자를 재정의한다.
	bool operator < ( const MyTask& rhs ) const;
};


/// 내 작업 포인터 타입 정의
typedef std::shared_ptr< MyTask > MyTaskPtr;
