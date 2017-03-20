#pragma once


#include <unordered_set>


class MyTask
{
private:
	/// tag set typedef
	typedef std::unordered_set< std::string > TagSet;

public:
	int         id;       ///< �ĺ���
	std::string name;     ///< �̸�
	int         priority; ///< �켱 ����
	TagSet      tagSet;   ///< tag set

public:
	/// ������
	MyTask( int id, const std::string& name );

	/// < �����ڸ� �������Ѵ�.
	bool operator < ( const MyTask& rhs ) const;
};


/// �� �۾� ������ Ÿ�� ����
typedef std::shared_ptr< MyTask > MyTaskPtr;
