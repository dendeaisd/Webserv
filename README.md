# Webserv

![](default/img/Webserv%20Lifecycle%20Overview.png)


# To pypass chrome CORS policy

Paste this in chrome and disable the attr: `chrome://flags/#block-insecure-private-network-requests`

reference: https://developer.chrome.com/blog/cors-rfc1918-feedback


# Benchmarking with siege
```bash
make docker-run
make re
./webserv
siege -b -c 100 -t 1m -T 2S http://127.0.0.1:8080/benchmark
```

# Curl with detailed reporting

let's agree, curl is cute
```bash
curl -v -s -w "\n-------\n\nDNS Lookup: %{time_namelookup}\nConnect: %{time_connect}\nStart Transfer: %{time_starttransfer}\nTotal: %{time_total}\n" http://localhost:8080/cgi-bin/blocking.py

curl -o /dev/null -s -w "DNS Lookup: %{time_namelookup}\nConnect: %{time_connect}\nStart Transfer: %{time_starttransfer}\nTotal: %{time_total}\n" http://localhost:8080/cgi-bin/blocking.py
```