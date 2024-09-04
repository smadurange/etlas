#!/usr/local/bin/python3

import os
from datetime import datetime, timedelta
from pathlib import Path
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

@app.route("/prices")
def get_stock_prices():
	global n, updated

	with mtx:
		ticker = tickers[n]["name"]
		price = tickers[n]["price"]

		if ticker in ts and updated + timedelta(hours=12) < datetime.now():
			return Response(prices[ticker], status=res.status_code, mimetype="text/plain")

		date = datetime.today()
		e_date = date.strftime("%Y-%m-%d")
		s_date = (date - timedelta(days=90)).strftime("%Y-%m-%d")

		base = "https://api.polygon.io/v2/aggs/ticker"
		query = f"adjusted=true&sort=asc&apiKey={api_key}"
		url = f"{base}/{ticker}/range/1/day/{s_date}/{e_date}?{query}"

		res = requests.get(url)

		result = f"{ticker}\n{price}\n"

		if res.status_code == 200:
			data = res.json()
			if "results" in data:
				for item in data["results"]:
					result += f"{item['c']:.2f}\n"
				ts[ticker] = result

				if len(ts) == 1:
					updated = datetime.now()

			n = (n + 1) % len(tickers)

		return Response(result, status=res.status_code, mimetype="text/plain")

if __name__ == '__main__':
    from flup.server.fcgi import WSGIServer
    sock = Path('/var/www/run/atlas.sock').resolve()
    try:
        sock.unlink()
    except:
        pass
    WSGIServer(app, bindAddress=str(sock), umask=0o007).run()
