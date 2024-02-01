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
void Login(const RbsLib::Network::HTTP::Request& request)
{
	std::cout << "login" << std::endl;
	std::cout << request.content.ToString() << std::endl;
	RbsLib::Network::HTTP::ResponseHeader header;
	header.status = 200;
	header.status_descraption = "ok";
	header.headers.AddHeader("Content-Type", "application/json");
	std::string json = "{\"message\":\"ok\",\"token\":\"12345\"}";
	header.headers.AddHeader("Content-Length", std::to_string(json.length()));
	request.connection.Send(header.ToBuffer());
	request.connection.Send(RbsLib::Buffer(json));
}

//��ʼ������������ģ������ĳ�ʼ������Ҫ������ģ�����ư汾��������Ϣ
ModuleSDK::ModuleInfo Init(void)
{
	//����ģ����Ϣ�ṹ�壬���ֱ�����ģ�����ơ��汾������
	ModuleSDK::ModuleInfo info("test", "1.0.0", "����");

	//Ϊģ�����һ������Ϊfunc�ķ�����������func����ʱ��ִ��func1����
	info.Add("func", func1);

	//�����һ������
	info.Add("login", Login);

	//��ģ����Ϣ����
	return info;
}