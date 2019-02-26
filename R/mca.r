huber <- function(e) {
	k <- 1.345 * sd(e)
	ret <- rep(NA, length(e))
	for (i in (1:length(e))) {
		if (abs(e[i]) <= k) {
			ret[i] <- 0.5 * (e[i] ^ 2)
		} else {
			ret[i] <- (k * abs(e[i])) - (0.5 * (k^2))
		}
	}
	return(ret)
}

cost <- function(b, x, y, m) {
	n <- length(y)
	e <- rep(NA, n)
	s <- 0
	for (j in (1:n)) {
		e[j] <- y[j] - ((m * x[j]) + b)
	}
	p <- huber(e)
	for (k in (1:n)) {
		s <- s + (p[k] * (0.75 ^ (x[n] - x[k])))
	}
	return(s)
}

get.e <- function(m, c) {
	n <- length(y)
	wghts <- rep(NA, length(b))
	e <- rep(NA, n)
	for (j in (1:n)) {
		e[j] <- y[j] - ((m * x[j]) + b)
	}
	p <- huber(e)
	return(p)
}

summarySE <- function(data=NULL, measurevar, groupvars=NULL, na.rm=FALSE, conf.interval=.95, .drop=TRUE) {
	require(plyr)
	# New version of length which can handle NA's: if na.rm==T, don't count them
	length2 <- function (x, na.rm=FALSE) {
		if (na.rm) sum(!is.na(x))
		else	length(x)
	}
	# This is does the summary; it's not easy to understand...
	datac <- ddply(data, groupvars, .drop=.drop, .fun= function(xx, col, na.rm) {
		c(N = length2(xx[,col], na.rm=na.rm), mean = mean(xx[,col], na.rm=na.rm), sd = sd(xx[,col], na.rm=na.rm))
        },
        measurevar,
        na.rm)
	# Rename the "mean" column    
	datac <- rename(datac, c("mean"=measurevar))
	return(datac)
}

plot.model = function(x, y, m, b, t_min, main="Relative Std. Dev. vs Virtual Precision") {
	new<-data.frame(x=seq(from=min(x), to=max(x), length.out=((max(x) - min(x)) + 1)))
	id <- seq(from=2, to=2, length.out=length(x))^-(min(x):max(x))	
	min <- min(c(min(id), min(10^y)))
	max <- max(c(max(id), max(10^y)))
	index <- which(x %in% t_min)
	plot(x[index:length(x)],10^y[index:length(x)], log="y", main=main, xlab="Virtual Precision (t)", ylab="Relative Std. Dev.", 
		ylim=c(min, max), xlim=c(min(x), max(x)), cex.lab=1.5, cex.axis=1.5, cex.main=1.5, cex.sub=1.5, pch=18, cex=0.75)
	abline(b_min$minimum, m)
	points(x[0:(index-1)], 10^y[0:(index-1)], pch=4)
	lines(x, id, lty=2)
	legend((max(x) - 15), max,c("MCA Data", "Outliers", "Linear Model", "Ideal"), lty = c(NA,NA,1,2), pch=c(18,4,NA,NA),col=c('black','black','black', 'black'),ncol=1)
}

find.tmin = function(m, c, mca.data) {
	margin <- log(2^0.5, 10)
	len <- length(mca.data$t)
	for (i in (len - 1):1) {
		predict <- (m * mca.data$t[i]) + c
		if (abs(predict - mca.data$log_rsd[i]) > margin) {
			return(mca.data$t[i + 1])
		}
	}
	return(1)
}

get.int = function(data, level = 0.95) {
	a <- mean(data)
	s <- sd(data)
	n <- length(data)
	c <- ((1 + level) / 2)
	err <- qnorm(c)*s/sqrt(n)
	return(err)
}

find.k = function(data, t_min) {
	K <- data[data$t >= t_min, ]
	K_2 <- log(K$rsd, 2) + K$t
	print(mean(K_2))
	print(get.int(K_2, level=0.95))
}

library(robustbase)
library(plyr)
par( mfrow = c( 1, 2) )
mca_res = read.csv(file="data/linpack.res")
mca_res <- summarySE(mca_res, measurevar="x", groupvars=c("Type", "t"))
mca_res <- mca_res[mca_res$sd >0, ]
mca_res <- mca_res[complete.cases(mca_res), ]
mca_res$rsd <- abs(mca_res$sd / mca_res$x)
mca_res$log_rsd <- log(mca_res$rsd, 10)
GRAD<-(-0.301029996)
max_t <- max(mca_res$t) - min(mca_res$t) + 1
y_lo <- (mca_res$log_rsd[max_t] - (GRAD * max(mca_res$t[max_t]))) + (2 * GRAD)
y_hi <- (mca_res$log_rsd[max_t] - (GRAD * max(mca_res$t[max_t]))) - (2 * GRAD)
b <- c(y_lo, y_hi)
b_min <- optimize(cost, b, x=mca_res$t, y=mca_res$log_rsd, m=GRAD)
t_min = find.tmin(GRAD, b_min$minimum, mca_res)
plot.model(mca_res$t, mca_res$log_rsd, GRAD, b_min, t_min, main="LINPACK Benchmark v. Precision")
find.k(mca_res, t_min)
