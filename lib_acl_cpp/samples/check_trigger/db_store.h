#pragma once

class http_thread;

class db_store
{
public:
	db_store();
	~db_store();

	// �������ݱ�
	bool db_update(const http_thread& http);

	// �������ݱ�
	bool db_create();

private:
};
