#!/usr/local/bin/python3

import os
from datetime import datetime, timedelta
from pathlib import Path
from threading import Lock

import requests
from flask import Flask, request, Response

app = Flask(__name__)
api_key = os.environ["POLYGON_API_KEY"]

log_file = "etlas.log"

def get_tickers():
	tab = []

	with open("tickers.txt", "r") as f:
		raw = f.read()
		
		with open(log_file, "a") as log:
			log.write(raw)

		lines = raw.split('\n')

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

@app.route("/prices")
def get_stock_prices():
	global n, updated

	with mtx:
		tickers = get_tickers()

		if len(tickers) == 0:
			return Response("", status=204, mimetype="text/plain")

		# tickers have been deleted from config
		if n > len(tickers) - 1:
			n = 0

		ticker = tickers[n]["name"]
		price = tickers[n]["price"]

		date = datetime.today()
		e_date = date.strftime("%Y-%m-%d")
		s_date = (date - timedelta(days=90)).strftime("%Y-%m-%d")

		base = "https://api.polygon.io/v2/aggs/ticker"
		query = f"adjusted=true&sort=asc&apiKey={api_key}"
		url = f"{base}/{ticker}/range/1/day/{s_date}/{e_date}?{query}"

		res = requests.get(url)

		with open(log_file, "a") as log:
			log.write("\n\n")
			log.write(res.text)

		result = f"{ticker}\n{price}\n"

		if res.status_code == 200:
			data = res.json()
			if "results" in data:
				for item in data["results"]:
					result += f"{item['c']:.2f}\n"

			n = (n + 1) % len(tickers)

		return Response(result, status=res.status_code, mimetype="text/plain")

if __name__ == '__main__':
    from flup.server.fcgi import WSGIServer
    sock = Path('/var/www/run/etlas.sock').resolve()
    try:
        sock.unlink()
    except:
        pass
    WSGIServer(app, bindAddress=str(sock), umask=0o007).run()
