import os
from datetime import datetime, timedelta
from pathlib import Path
from threading import Lock

import requests
from flask import Flask, request, Response

app = Flask(__name__)

n = 0
mtx = Lock()
tickers = Path('tickers.txt').read_text().split("\n")

@app.route("/stock-prices")
def get_stock_prices():
	global n

	api_key = os.environ["POLYGON_API_KEY"]
	base = "https://api.polygon.io/v2/aggs/ticker"

	date = datetime.today()
	e_date = date.strftime("%Y-%m-%d")
	s_date = (date - timedelta(days=30)).strftime("%Y-%m-%d")

	with mtx:
		ticker = tickers[n]
		url = f"{base}/{ticker}/range/1/day/{s_date}/{e_date}?adjusted=true&sort=asc&apiKey={api_key}"
		n = (n + 1) % len(tickers)

		res = requests.get(url)

		if res.status_code == 200:
			txt = f"{ticker}\n"
			data = res.json()
			for item in data["results"]:
				txt += f"{item['c']:.2f}\n"
			return Response(txt, status=res.status_code, mimetype="plain/txt")
		else:
			return Response(res.txt, status=res.status_code, mimetype="plain/text")
