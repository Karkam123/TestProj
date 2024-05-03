#include "udp_handler.h"

#include "udp.h"

#include <string.h>


static struct udp_pcb* upcb = NULL;

// static void udp_receive_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p,
// 	    const ip_addr_t *addr, u16_t port)
//{
	// в этой функции обязательно должны очистить p, иначе память потечёт
//	pbuf_free(p);
//}


struct udp_pcb* udp_create_socket(const ip4_addr_t ip_addr, const u16_t port, udp_recv_fn recv, void *recv_arg)
{

	// проверяем, что не инициализировали сокет еще
	if (upcb == NULL)
	{
		// создание сокета
		upcb = udp_new();

		// если не удалось создать сокет, то на выход с ошибкой
		if (upcb == NULL)
		{
			return NULL;
		}

	}

	//ip4_addr_t dest;
	//IP4_ADDR(&dest, 192, 168, 0, 11);
	// коннектимся к удаленному серверу по ИП и порту (сервер должен быть настроен именно на так)
	err_t err = udp_connect(upcb, &ip_addr, port);
	if (ERR_OK != err)
	{
		return NULL;
	}

	// регистрируем колбэк на прием пакета
	udp_recv(upcb, recv, recv_arg);

	return upcb;
}

err_t udp_send_msg(struct udp_pcb* upcb, const char* data)
{
	int razmer = strlen(data);
	// если сокет не создался, то на выход с ошибкой
	if (upcb == NULL)
	{
		return ERR_ABRT;
	}
	// аллоцируем память под буфер с данными
	struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, razmer, PBUF_RAM);
	if (p != NULL)
	{
		//char data[5] = "Test";
		// кладём данные в аллоцированный буфер
		err_t err = pbuf_take(p, data, razmer);
		if (ERR_OK != err)
		{
			// обязательно должны очистить аллоцированную память при ошибке
			pbuf_free(p);
			return err;
		}

		// отсылаем пакет
		err = udp_send(upcb, p);
		if (ERR_OK != err)
		{
			// обязательно должны очистить аллоцированную память при ошибке
			pbuf_free(p);
			return err;
		}
		// очищаем аллоцированную память
		pbuf_free(p);
	}
	return ERR_OK;
}
