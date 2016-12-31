# trading-stat
Trading-Stat is a project to get statistical report of stock market. Idea behind this project is to fetch information from twitter like social media sources and provide popularity info about stock of interest to user.
There are multiple components associated with this project,
* Logstash : Logstash is an open source tool for collecting, parsing, and storing logs for future use. this tol will handle Twitter social media data and stores into Elasticsearch.
* Elasticsearch : Elasticsearch is a search engine based on Lucene. It provides a distributed, multitenant-capable full-text search engine with an HTTP web interface and schema-free JSON documents.
* User Interface : This UI component will bind both "Stock Market Data" and "Popularity Data" and presents to user in a formatted way.
* Yahoo Finance : This is the stock market data source where the stock information will be fetched from.
* Stock Analyser : This library helps parsing and formatting data fetched from Yahoo Finance.

![Alt text](./trading-stat.jpg?raw=true "Block diagram of Trading-Stat project")

Complete project is under development and only few components are finished.
I will push to repository as and when I finish my development and testing.
