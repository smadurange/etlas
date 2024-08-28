import os
from datetime import datetime, timedelta
from threading import Lock

import requests
from passlib.hash import pbkdf2_sha256
from flask import Flask, request, Response

app = Flask(__name__)

pwd = os.environ["ATLAS_KEY_HASH"]
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
mtx = Lock()
tickers = get_tickers()

@app.route("/stock-prices")
def get_stock_prices():
	global n

	auth_key = request.headers.get("x-api-key")

	if not pbkdf2_sha256.verify(auth_key, pwd):
		return Response("Unauthorized\n",
			status=401,
			mimetype="plain/text")

	base = "https://api.polygon.io/v2/aggs/ticker"

	date = datetime.today()
	e_date = date.strftime("%Y-%m-%d")
	s_date = (date - timedelta(days=30)).strftime("%Y-%m-%d")

	with mtx:
		ticker = tickers[n]["name"]
		price = tickers[n]["price"]
		result = f"{ticker}\n{price}\n"

		url = f"{base}/{ticker}/range/1/day/{s_date}/{e_date}?adjusted=true&sort=asc&apiKey={api_key}"
		n = (n + 1) % len(tickers)

		res = requests.get(url)

		if res.status_code == 200:
			data = res.json()
			for item in data["results"]:
				result += f"{item['c']:.2f}\n"

		return Response(result, status=res.status_code, mimetype="plain/text")

