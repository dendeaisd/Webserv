# Webserv

![](img/Webserv%20Lifecycle%20Overview.png)


# To pypass chrome CORS policy

Paste this in chrome and disable the attr: `chrome://flags/#block-insecure-private-network-requests`

reference: https://developer.chrome.com/blog/cors-rfc1918-feedback


# Benchmarking with siege
`make docker-run`
`make re`
`./webserv`
`siege -b -c 100 -t 1m -T 2S http://127.0.0.1:8080/benchmark`