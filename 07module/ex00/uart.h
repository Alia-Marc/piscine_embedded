


void	uart_init();
char	uart_rx(void);
void	uart_tx(char c);
void	uart_printstr(const char *str);
void	uart_printnbr(int n);
void	uart_printHEX8b(char c);
void	uart_printhex8b(char c);
bool	is_printable(char c);