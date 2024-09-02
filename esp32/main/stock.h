#ifndef STOCK_H
#define STOCK_H

#define TICKER_LEN 10
#define PRICE_VAL_LEN 10
#define PRICE_DATA_LEN 90

struct stock_data {
	char ticker[TICKER_LEN];
	int *prices;
	int price_min;
	int price_max;
	int price_ref;
};

void stock_init(void);

void stock_get_data(struct stock_data *sd);

#endif /* STOCK_H */
