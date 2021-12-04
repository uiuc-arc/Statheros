// Inferring the Mean and Standard Deviation of a Gaussian
data { 
  int<lower=1> n;
  vector<lower=0>[n] x;
}
parameters {
  real mu;
  real<lower=0,upper=10> sigma; 
} 
model {
  // Priors
  mu ~ normal(0, sqrt(1000));
  // Data Come From A Gaussian
  x ~ normal(mu, sigma);
}
