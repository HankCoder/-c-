#pragma once
#include "acl_cpp/stdlib/pipe_stream.hpp"

//class acl::string;

class pipeline_string : public acl::pipe_stream
{
public:
	pipeline_string();
	~pipeline_string();

protected:
	/**
	* ������������ӿ�
	* @param in {const char*} �������ݵĵ�ַ
	* @param len {size_t} �������ݳ���
	* @param out {string*} �洢������������������Ϊ��
	* @param max {size_t} ϣ�����յ��������ĳ������ƣ����Ϊ0��
	*  ��ʾû�����ƣ����������洢�� out ��������
	* @return {int} ������ݵĳ��ȣ���� < 0 ���ʾ����
	*/
	virtual int push_pop(const char* in, size_t len,
		acl::string* out, size_t max = 0);

	/**
	* ������������ݽӿ�
	* @param out {string*} �洢������������������Ϊ��
	* @param max {size_t} ϣ�����յ��������ĳ������ƣ����Ϊ0��
	*  ��ʾû�����ƣ����������洢�� out ��������
	* @return {int} ������ݵĳ��ȣ���� < 0 ���ʾ����
	*/
	virtual int pop_end(acl::string* out, size_t max = 0);

	/**
	* ����ڲ�������
	*/
	virtual void clear();
private:
	bool strip_sp_;
};
