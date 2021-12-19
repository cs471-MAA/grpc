
#ifdef LOCALHOST
#define M_GET_SOCKET_ADDRESS(ADDRESS, PORT) "0.0.0.0:" PORT
#else
#define M_GET_SOCKET_ADDRESS(ADDRESS, PORT) ADDRESS ":" PORT
#endif

#define MOCK_DATABASE_HOSTNAME "mock-database"
#define MOCK_DATABASE_PORT "20001"
#define MESSAGE_SERVICE_HOSTNAME "message-service"
#define MESSAGE_SERVICE_PORT "20002"
#define SANITIZATION_SERVICE_HOSTNAME "sanitization-service"
#define SANITIZATION_SERVICE_PORT "20003"

#define M_MOCK_DATABASE_SOCKET_ADDRESS M_GET_SOCKET_ADDRESS(MOCK_DATABASE_HOSTNAME, MOCK_DATABASE_PORT)
#define M_MESSAGE_SERVICE_SOCKET_ADDRESS M_GET_SOCKET_ADDRESS(MESSAGE_SERVICE_HOSTNAME, MESSAGE_SERVICE_PORT)
#define M_SANITIZATION_SERVICE_SOCKET_ADDRESS M_GET_SOCKET_ADDRESS(SANITIZATION_SERVICE_HOSTNAME, SANITIZATION_SERVICE_PORT)