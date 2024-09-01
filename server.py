import os
from datetime import datetime, timedelta
from threading import Lock

import requests
from flask import Flask, request, Response

app = Flask(__name__)
api_key = os.environ["POLYGON_API_KEY"]

def get_tickers():
	tab = []

	with open("tickers.txt", "r") as f:
		lines = f.read().split('\n')
		for line in lines:
			if line:
				parts = line.split()
				tab.append({
					"name": parts[0],
					"price": parts[1]
				})

	return tab

n = 0
ts = {}
mtx = Lock()
tickers = get_tickers()
updated = datetime.now()
update_interval = 60 * 15

@app.route("/prices")
def get_stock_prices():
	global n

	with mtx:
		ticker = tickers[n]["name"]
		price = tickers[n]["price"]

		if ticker in ts and updated + timedelta(seconds=update_interval) < datetime.now():
			print("from cache " + tick)
			return Response(prices[ticker], status=res.status_code, mimetype="text/plain")

		date = datetime.today()
		e_date = date.strftime("%Y-%m-%d")
		s_date = (date - timedelta(days=30)).strftime("%Y-%m-%d")

		base = "https://api.polygon.io/v2/aggs/ticker"
		url = f"{base}/{ticker}/range/1/day/{s_date}/{e_date}?adjusted=true&sort=asc&apiKey={api_key}"

		res = requests.get(url)

		result = f"{ticker}\n{price}\n"

		if res.status_code == 200:
			data = res.json()
			for item in data["results"]:
				result += f"{item['c']:.2f}\n"
			ts[ticker] = result

			n = (n + 1) % len(tickers)

		return Response(result, status=res.status_code, mimetype="text/plain")

