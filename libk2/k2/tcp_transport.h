#ifndef	K2_TCP_TRANASPORT
#define	K2_TCP_TRANASPORT

#ifndef	K2_CONFIG_H
#	include <k2/config.h>
#endif
#ifndef	K2_SOCKET_H
#	include <k2/socket.h>
#endif



namespace k2
{

	namespace ipv4
	{

		class tcp_listener
		{
		public:
			explicit tcp_listener (const layer4_addr& local_addr);

		protected:
			int	accept ();
			int accept (const time_span& timeout);

			friend class tcp_transport;

		private:
			socket	m_sock;
		};

		class tcp_transport
		{
		public:
			explicit tcp_transport (const layer4_addr& remote_addr);
			explicit tcp_transport (const layer4_addr& remote_addr, const time_span& timeout);

			tcp_transport (const layer4_addr& local_addr, const layer4_addr& remote_addr);
			tcp_transport (const layer4_addr& local_addr, const layer4_addr& remote_addr, const time_span& timeout);

			explicit tcp_transport (tcp_listener& listener);
			tcp_transport (tcp_listener& listener, const time_span& timeout);

			const layer4_addr	local_addr () const;
			const layer4_addr	remote_addr () const;

			size_t	write (const char* buf, size_t bytes);
			size_t	write_all (const char* buf, size_t bytes);
			size_t	write_all (const char* buf, size_t bytes, const time_span& timeout);

			size_t	read (char* buf, size_t bytes);
			size_t	read_all (char* buf, size_t bytes);
			size_t	read_all (char* buf, size_t bytes, const time_span& timeout);

		private:
			socket	m_sock;
		};

		class udp_transport
		{
		public:
			explicit udp_transport ();
			explicit udp_transport (const layer4_addr& local_addr);

			const layer4_addr	local_addr () const;

			size_t	write_to (const char* buf, size_t bytes, const layer4_addr& remote_addr);

			size_t	read_from (char* buf, size_t bytes, layer4_addr& remote_addr);
			size_t	read_from (char* buf, size_t bytes, layer4_addr& remote_addr, const time_span& timeout);

		private:
			socket	m_sock;
		};

	}

}	//	namespace k2

#endif	//	K2_TCP_TRANASPORT
