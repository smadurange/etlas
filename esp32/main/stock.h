#ifndef STOCK_H
#define STOCK_H

#define TICKER_LEN 25

struct stock_data {
	char ticker[TICKER_LEN];
	int *prices;
	int price_min;
	int price_max;
	int price_ref;
	int prices_len;
	int prices_maxlen;
};

void stock_get_data(struct stock_data *sd);

#endif /* STOCK_H */
