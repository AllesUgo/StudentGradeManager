#include "module_sdk.h"
#include <iostream>

//��������
void func1(const RbsLib::Network::HTTP::Request& request)
{
	std::cout << "test" << std::endl;
	request.connection;//����TCP���ӣ���ͻ��˷���������������
	request.header;//�ͻ��˷��͵�HTTP�����ͷ
	request.content;//�ͻ��˷��͵�HTTP��������(��POST������Ч)
}
void func2(const RbsLib::Network::HTTP::Request& request)
{
	std::cout << "test2" << std::endl;
}

//��ʼ������������ģ������ĳ�ʼ������Ҫ������ģ�����ư汾��������Ϣ
ModuleSDK::ModuleInfo Init(void)
{
	//����ģ����Ϣ�ṹ�壬���ֱ�����ģ�����ơ��汾������
	ModuleSDK::ModuleInfo info("test", "1.0.0", "����");

	//Ϊģ�����һ������Ϊfunc�ķ�����������func����ʱ��ִ��func1����
	info.Add("func", func1);

	//�����һ������
	info.Add("func2", func2);

	//��ģ����Ϣ����
	return info;
}