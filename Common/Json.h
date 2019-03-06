#pragma once

#define _CRT_SECURE_NO_WARNINGS

#define JSON_VAL_STRING  1
#define JSON_VAL_INTER   2
#define JSON_VAL_DECIMAL 3
#define JSON_VAL_NODE    4
#define JSON_VAL_LIST    5

typedef unsigned int u_int;

typedef struct p_json_sign {
	char  sign;
	char* start;
	char* end;
} PJsonSign;

typedef struct json_value {
	char* key;
	char* value;
	json_value* child;
	json_value* next;
} JsonValue;

typedef struct json_node {
	char* key;               //��ֵ
	u_int key_addlen;           //��ֵ����
	union {
		char*      str;      //�ַ���
		int        inter;    //����
		double     decimal;  //С��
		json_node* child; //��һ������
	} value;                 //ֵ
	u_int value_addlen;         //ֵ����
	u_int type;              //1-�ַ���, 2-����, 3-С��, 4-��json����, 5-�б�
	json_node* next;         //��һ������
} JsonNode;


class Json {
private:
	char*      _string;
	JsonValue* _phead;
	u_int      _addnum;
	u_int      _addlen;    //ת��jsonҪ����ĳ���
	JsonNode*  _addhead;   //����ͷ
	JsonNode*  _addcurr;   //��ǰ
private:
	char *_getChildValue(char** ptr, int curr, int total, JsonValue* child);
	bool  _add(JsonNode* p, u_int num); //�������, num[Ҫ����ĳ���]
	char* _toJson(char* str, JsonNode* node); //ת��json
public:
	Json();
	int parseString(char* string, JsonValue** node = nullptr, bool is_child = false); //�����ַ���
	void echoV();
	JsonValue*  getJsonValue(char* key) const;  //���ݼ�ֵ��ȡJsonValue, ���ȵ���parseString�����ַ���
	char*  valueOfChild(char* key);
	char*  value(char* key);             //���ݼ�ֵ��ȡֵ, ���ȵ���parseString�����ַ���
	int valueToInt(char* key);
	float valueToFloat(char* key);
	double valueToDouble(char* key);
	JsonNode* createList(bool add = false); //����һ��json�б�
	JsonNode* push(char* key, char* value, bool add = true); //���json��
	JsonNode* push(char* key, int value, bool add = true); //ͬ��
	JsonNode* push(char* key, double value, bool add = true); //ͬ��
	JsonNode* push(char* key, JsonNode* node, bool add = true); //ͬ��
	JsonNode* push(JsonNode* parent, JsonNode* child); //ͬ��
	JsonNode* push(JsonNode* node); //ͬ��
	char* toJson(); //ת��json
	void clear(JsonValue* p = nullptr); //����json, ת��json������ô˺���
	void clearAdd(JsonNode* p = nullptr); //����json, ת��json������ô˺���
};