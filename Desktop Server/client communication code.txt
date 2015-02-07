		else {								//close if errors
				printf("receive failed1: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}


		if (result > 0) {											//if message was received
			strcpy(sendbuffer, "Client says thanks for: ");
			strcat(sendbuffer, recvbuffer);
			result = strlen(sendbuffer);

			iSendResult = send(ClientSocket, sendbuffer, result, 0);//send message
			if (iSendResult == SOCKET_ERROR) {						//close if errors
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			printf("\nBytes sent: %d\n", iSendResult);

		}
		else if (result == 0)										//close connection if shut down by client
			printf("Connection closing...\n");
		else {														//quit on receive error
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}


	} while (result > 0);		//repeat while client sends

	//shutdown the sending half of the client connection
	result = shutdown(ClientSocket, SD_SEND);
	if (result == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	//close remaining socket components
	closesocket(ClientSocket);		//closes socket
