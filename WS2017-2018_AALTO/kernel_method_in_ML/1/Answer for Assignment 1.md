# Answer for Assignment 1


Dongmin Wu, 605308, Aalto University


## Pen and perpaer exercise

### Question 1:

No, if there is a matrix like this:


\[
K = \begin{bmatrix}
       1 & -1          \\
       -1 & 1 			
     \end{bmatrix}
\]

This matrix is a positive semidefinite matrix, but not all of its entries are positive. 

### Question 2:

Yes, according to the defination of Positive semi-definite matrix \(A\), for any vector \(v' \in \mathbb{R}^{n \times n}\), we have \(v'Av \geq 0\).

So we can get 
\[
v'K_1v \geq 0, v'K_2v \geq 0
\]

For \(K\), we just need to verify if the \(v'Kv \geq 0\) or not.



\[
\begin{split}
 v'Kv & = v'(aK_1 + bK_2)v \\
  & = av'K_1v + bv'K_2v
\end{split}
\]

Because \(a,b \in \mathbb{R}^+\), we can derive 

\[
av'K_1v + bv'K_2v \geq 0
\]

which means \(v'Kv \geq 0\) is verified and \(K\) is a vaild kernel matrix.

### Question 3

No, probably get negative result

Considering the situation below: 

\[
K_1 = \begin{bmatrix}
       1 & 0          \\
       0 & 1 			
     \end{bmatrix} ,   
 K_2 = \begin{bmatrix}
       3 & 0         \\
       0 & 3			
     \end{bmatrix}
\] 

We can get 

\[
 K = \begin{bmatrix}
       -2 & 0         \\
       0 & -2			
     \end{bmatrix}
\] 

Whose eigenvalue is negative, which is not a vaild kernel matrix

### Question 4

\[
  \begin{split}
h(x) & = sgn(||\phi(x) - c_-||^2 - ||\phi(x)-c_+||^2) \\
& = sgn(\phi(x)^2 - 2\phi(x)c_- + c_-^2 - \phi(x)^2  + 2\phi(x)c_+ - c_+^2 ) \\
& = sgn(-2\phi(x)c_- + c_-^2  + 2\phi(x)c_+ - 2c_+^2) \\
& =  sgn(\frac12 \times (
  \frac{-1}{m_-} \Sigma_{i\in I^-} \langle \phi(x), \phi(x_i)\rangle + 
  \frac{1}{2m_-^2} \Sigma_{i,j\in I^-}\langle\phi(x_i),\phi(x_j)\rangle  + 
  \frac{1}{m_+} \Sigma_{i\in I^+} \langle \phi(x), \phi(x_i)\rangle - 
  \frac{1}{2m_+^2} \Sigma_{i,j\in I^+}\langle\phi(x_i),\phi(x_j)\rangle ) )\\
& =  sgn(\frac12 \times (
   \Sigma_{i\in I} \alpha_i \langle \phi(x), \phi(x_i)\rangle + 
  \frac{1}{2m_-^2} \Sigma_{i,j\in I^-}\langle\phi(x_i),\phi(x_j)\rangle  -
  \frac{1}{2m_+^2} \Sigma_{i,j\in I^+}\langle\phi(x_i),\phi(x_j)\rangle ) )\\
& =  sgn(\frac12 \times (
   \Sigma_{i\in I} \alpha_i \kappa( \phi(x), \phi(x_i))+ 
  \frac{1}{2m_-^2} \kappa(\phi(x_i),\phi(x_j))  -
  \frac{1}{2m_+^2} \kappa(\phi(x_i),\phi(x_j)) ) )\\
& =  sgn(\frac12 \times (  \Sigma_{i\in I} \alpha_i \kappa( \phi(x), \phi(x_i))+ 
  \frac{1}{2m_-^2} \kappa(\phi(x_i),\phi(x_j))  -
  \frac{1}{2m_+^2} \kappa(\phi(x_i),\phi(x_j)) ) )\\
& =  sgn(\frac12 \times (  \Sigma_{i\in I} \alpha_i \kappa( \phi(x), \phi(x_i))+ 
  b ) )
  \end{split}   
\]

we can easily eminate the coefficient \(\frac12\) because of the property of sign function.

In conlution, we can get 
\[
  h(x) =  sgn(||\phi(x) - c_-||^2 - ||\phi(x)-c_+||^2) = sgn( \Sigma_{i\in I} \alpha_i \kappa( \phi(x), \phi(x_i))+ 
  b ) 
  \]