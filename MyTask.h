#pragma once


class MyTask
{
public:
	int         id;       ///< �ĺ���
	std::string name;     ///< �̸�
	int         priority; ///< �켱 ����

public:
	/// ������
	MyTask( int id, const std::string& name );

	/// < �����ڸ� �������Ѵ�.
	bool operator < ( const MyTask& rhs ) const;
};


/// �� �۾� ������ Ÿ�� ����
typedef std::shared_ptr< MyTask > MyTaskPtr;
