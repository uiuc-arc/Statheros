
// Repeated Measures of IQ
data { 
  int<lower=1> n;
  int<lower=1> m;
  matrix[n, m] x;
}
parameters {
  vector<lower=0,upper=300>[n] mu;
  real<lower=0,upper=100> sigma;
} 
model {
  // Data Come From Gaussians With Different Means But Common Standard Deviation
  for (i in 1:n)
    for (j in 1:m)  
      x[i,j] ~ normal(mu[i], sigma);
}
