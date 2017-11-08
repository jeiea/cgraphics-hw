#pragma once
/*******************************************************************************
* Author        : 2013726081 Kim Hojun
* Modified Date : 2017-09-16
* Description   : Matrix class
* Tested on Visual studio 2017 15.3.5
******************************************************************************/
#include <algorithm>
#include <iostream>
#include <numeric>
#include <iterator>
#include <random>
#include <sstream>
#include <tuple>
#include <vector>

using namespace std;

template <typename T>
class matrix_row {
public:
  matrix_row(
    typename vector<T>::iterator beg,
    typename vector<T>::iterator end)
  {
    this->_beg = beg;
    this->_end = end;
    cbeg = beg;
    cend = end;
  }

  matrix_row(
    typename vector<T>::const_iterator beg,
    typename vector<T>::const_iterator end)
  {
    cbeg = beg;
    cend = end;
  }

  typename vector<T>::iterator begin() {
    return _beg;
  }

  typename vector<T>::iterator end() {
    return _end;
  }

  T& operator [](int i) {
    return *(_beg + i);
  }

  T& operator *() {
    return *_beg;
  }

  operator T*() {
    return &*_beg;
  }

  operator const T*() {
    return &*_beg;
  }

  typename vector<T>::const_iterator begin() const {
    return cbeg;
  }

  typename vector<T>::const_iterator end() const {
    return cend;
  }

  const T& operator [](int i) const {
    return *(cbeg + i);
  }

  const T& operator *() const {
    return *cbeg;
  }

  operator const T*() const {
    return &*cbeg;
  }

private:
  typename vector<T>::iterator _beg;
  typename vector<T>::iterator _end;
  typename vector<T>::const_iterator cbeg;
  typename vector<T>::const_iterator cend;
};

template <typename T>
ostream& operator <<(ostream& out, const matrix_row<T>& m) {
  out << '(';

  typename vector<T>::const_iterator it = m.begin();
  int col = static_cast<int>(distance(it, m.end()));

  if (col) out << *it++;
  for (int j = 1; j < col; j++)
  {
    out << ", " << *it++;
  }

  return out << ')';
}

/// <summary>   matrix class. </summary>
template <typename T>
class matrix
{
public:
  // Constructors and destructor

  // Generates 0x0 matrix.
  matrix();

  // Generates a matrix
  matrix(const initializer_list<const initializer_list<T>> m);

  // Generates a matrix that is the same as m.
  matrix(const matrix<T>& m);

  // Generates a matrix that is the same as m.
  matrix(matrix&& m);

  // Generates a matrix whose column size is c.
  template<typename U>
  matrix(int c, const U& beg, const U& end) {
    reserve(static_cast<int>(distance(beg, end)) / c, c);
    copy(beg, end, back_inserter(elements));
  }

  // Precondition: r, c are non-negative
  // Postcondition: r x c matrix whose all value is element_value.
  matrix(int r, int c, const T& element_value);

  // Generates an n x n matrix whose all the diagonal values are diagonal_value
  // and the other values are zero (false).
  // Precondition: n is non-negative.
  // Postcondition: When diagonal_value is false, n x n zero matrix If not, n x n identity matrix.
  matrix(int n, const T& diagonal_value);

  // Postcondition: Free all dynamically allocated memory.
  virtual ~matrix();

  // Member functions

  // Precondition: 0 <= i < rowN and 0 <= j < colN.
  // Postcondition: (i, j)-th bool reference. If out-of-range, It'll be clamped.
  T& at(int i, int j);
  const T& at(int i, int j) const;

  // Postcondition: Transposed matrix.
  const matrix<T> transpose() const;

  // Precondition: matrix is invertible.
  // Postcondition: Inversed matrix. If the matrix is singular, it returns matrix(0).
  const matrix<T> inverse() const;

  // Precondition: rowN == colN.
  // Postcondition: The n-th power of the matrix. If error occurs, it returns matrix(0).
  const matrix<T> power(int n) const;

  // Postcondition: rowN = r, colN = c.
  tuple<int, int> size() const;

  // Postcondition: true if m is identity matrix.
  bool is_identity() const;

  // Additional member function.

  // Postcondition: true if it's zero matrix.
  bool is_zero() const;

  // return r x c random matrix.
  // Precondition: r, c are non-negative.
  // Postcontition: r x c random matrix.
  static matrix<T> rand(int r, int c);

  // You may use this for error checking. matrix(0).area() is 0;
  // Postcondition: rowN * colN
  int area() const;

  // It returns "1 0\n0 1" form string.
  string str() const;

  // Read str() form string.
  static matrix<T> fromStr(const string& src);

  // Operators
  // Cast operator
  template <typename U>
  operator matrix<U>() const {
    return matrix<U> { colN, elements.cbegin(), elements.cend() };
  }

  // Addition, subtraction, dot product, division operator.
  template <typename T>
  friend matrix<T> operator +(const matrix<T>& a, const matrix<T>& b);
  template <typename T>
  friend matrix<T> operator -(const matrix<T>& a, const matrix<T>& b);
  template <typename T>
  friend matrix<T> operator *(const matrix<T>& a, const matrix<T>& b);
  template <typename T>
  friend matrix<T> operator /(const matrix<T>& a, const matrix<T>& b);
  // cross product for vector
  template <typename T>
  friend matrix<T> operator %(const matrix<T>& a, const matrix<T>& b);
  // Constant multiplication, division
  template <typename T, typename K>
  friend matrix<T> operator *(const K& coef, const matrix<T>& mat);
  template <typename T, typename K>
  friend matrix<T> operator *(const matrix<T>& mat, const K& coef);
  template <typename K>
  matrix<T> operator /(const K& divisor) const;

  // Unary minus operator.
  matrix<T> operator -() const;

  // Additional array subscript operator. These don't check range.
  matrix_row<T> operator [](int i);
  const matrix_row<T> operator [](int i) const;

  // (Compound) assignment operators
  matrix<T>& operator  =(const matrix<T>& other);
  matrix<T>& operator  =(matrix<T> && other);
  matrix<T>& operator +=(const matrix<T>& other);
  matrix<T>& operator -=(const matrix<T>& other);
  matrix<T>& operator *=(const matrix<T>& other);
  matrix<T>& operator /=(const matrix<T>& other);

  // Comparison operators
  friend bool operator ==(const matrix<T>& a, const matrix<T>& b);
  friend bool operator !=(const matrix<T>& a, const matrix<T>& b);
  friend bool operator < (const matrix<T>& a, const matrix<T>& b);
  friend bool operator <=(const matrix<T>& a, const matrix<T>& b);
  friend bool operator > (const matrix<T>& a, const matrix<T>& b);
  friend bool operator >=(const matrix<T>& a, const matrix<T>& b);

  // Stream operator
  template <typename T>
  friend std::ostream& operator <<(std::ostream& out, const matrix<T>& m);
  template <typename T>
  friend std::istream& operator >>(std::istream& in, matrix<T>& m);

  void reserve(int row, int col);

private:
  int rowN;
  int colN;

  // Array containing contents.
  vector<T> elements;

  // Random engine. initialize manual.
  static std::mt19937 engine;

  // Internal function which helps Gauss-Jordan elimination.
  // Precondition: First row properly pivoted.
  // Postcondition: Row operation. (idx, pivot)-th value will be 0.
  inline static void eliminate(matrix<T>& m, int idx, int pivot);
};

/// <summary>   Generates 0x0 matrix&lt;T&gt;. </summary>
template<typename T>
matrix<T>::matrix()
{
  rowN = colN = 0;
}

template<typename T>
matrix<T>::matrix(const initializer_list<const initializer_list<T>> m) {

  if (!m.size() || !m.begin()->size()) {
    matrix();
    return;
  }

  rowN = static_cast<int>(m.size());
  colN = static_cast<int>(m.begin()->size());
  elements.reserve(rowN * colN);

  for (auto row : m) {
    elements.insert(elements.end(), row);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Generates a matrix&lt;T&gt; that is the same as m. </summary>
///
/// <param name="m">    Original instance. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
matrix<T>::matrix(const matrix<T>& m)
{
  rowN = m.rowN;
  colN = m.colN;
  elements = m.elements;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Additional copy constructor for performance. </summary>
///
/// <param name="m">    [in,out] Temporary instance. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
matrix<T>::matrix(matrix<T>&& m)
{
  rowN = m.rowN;
  colN = m.colN;
  swap(elements, m.elements);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Generates an m x n matrix&lt;T&gt; whose all the values are element_value. </summary>
///
/// <param name="r">            Row size. </param>
/// <param name="c">            Column size. </param>
/// <param name="element_value"> The value which fills whole matrix&lt;T&gt;. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
matrix<T>::matrix(int r, int c, const T& element_value)
{
  if (r > 0 && c > 0) {
    rowN = r;
    colN = c;
  }
  else {
    rowN = colN = 0;
  }

  elements.resize(rowN * colN, element_value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Generates an n x n matrix&lt;T&gt; whose all the diagonal values are diagonal_value and the other
/// values are zero (false).
/// </summary>
///
/// <param name="n">                Side size. </param>
/// <param name="diagonal_value">    (i, i)-th value. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
matrix<T>::matrix(int n, const T& diagonal_value)
{
  rowN = colN = max(n, 0);
  int size = rowN * colN;
  elements.resize(size);

  for (int i = 0; i < size; i += colN + 1) {
    elements[i] = diagonal_value;
  }
}

/// <summary>   Free all dynamically allocated memory. </summary>
template<typename T>
matrix<T>::~matrix()
{
  elements.clear();
}

// Member functions

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Returns the reference of the (i, j)-th element. </summary>
///
/// <remarks>   If i or j are out of range, it returns clamped index value. </remarks>
///
/// <param name="i">    Row index. </param>
/// <param name="j">    Column index. </param>
///
/// <returns>   (i, j)-th value T&amp; </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
T& matrix<T>::at(int i, int j)
{
  int r = max(min(i, rowN - 1), 0);
  int c = max(min(j, colN - 1), 0);

  if (r != i || c != j)
  {
    const char* msg = "matrix index out of range.";
    cerr << msg << endl;
    throw msg;
  }

  return (*this)[r][c];
}

template <typename T>
const T& matrix<T>::at(int i, int j) const
{
  return const_cast<matrix<T>*>(this)->at(i, j);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Return transposed matrix&lt;T&gt;. </summary>
///
/// <returns>   Transposed matrix&lt;T&gt;. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
const matrix<T> matrix<T>::transpose(void) const
{
  matrix<T> m;
  m.reserve(colN, rowN);

  // m에 글 읽는 순서로 채워넣음
  for (int i = 0; i < colN; i++)
  {
    for (int j = 0; j < rowN; j++)
    {
      m.elements.push_back((*this)[j][i]);
    }
  }
  return m;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Returns the inverse matrix&lt;T&gt;. </summary>
///
/// <remarks>   If the matrix&lt;T&gt; is singular, it returns matrix&lt;T&gt;(0) with error message. </remarks>
///
/// <returns>   (this matrix&lt;T&gt;) ^ (-1) </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
const matrix<T> matrix<T>::inverse() const
{
  if (rowN != colN) {
    cerr << "inverse ASSERT: matrix<T> is not square." << endl;
    return matrix<T>();
  }

  // Create augmented matrix<T>.
  matrix<T> augment(rowN, colN * 2, T());
  for (int i = 0; i < rowN; i++) {
    // Copy elements.
    auto row = (*this)[i];
    auto dst = augment[i];
    copy(row.begin(), row.end(), dst.begin());

    // Augment I element.
    augment[i][colN + i] = 1;
  }

  // Start Gauss-Jordan Elimination with partial pivoting.
  for (int i = 0; i < rowN; i++) {
    // Pivoting
    if (augment[i][i] == 0) {
      int pivot = i;

      for (int j = i + 1; j < rowN; j++) {
        if (augment[j][i]) {
          pivot = j;
          break;
        }
      }

      if (pivot == i) {
        cerr << "inverse: inverse matrix<T> is not exists." << endl;
        return matrix<T>();
      }

      // row swap
      swap_ranges(augment[i].begin(), augment[i].end(), augment[pivot].begin());
    }

    // Elimination
    for (int j = 0; j < i; j++) {
      eliminate(augment, j, i);
    }
    for (int j = i + 1; j < rowN; j++) {
      eliminate(augment, j, i);
    }
  }

  matrix<T> result(rowN, T());
  for (int i = 0; i < rowN; i++) {
    copy(augment[i].begin() + colN, augment[i].end(), result[i].begin());
  }

  return result;
}

template <typename T>
inline void matrix<T>::eliminate(matrix<T>& m, int idx, int pivot)
{
  // already eliminated
  if (m[idx][pivot] == T()) return;

  transform(m[idx].begin(), m[idx].end(),
    m[pivot].begin(), m[idx].begin(), minus<T>());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Return the n-th power of the matrix&lt;T&gt; if possible. </summary>
///
/// <param name="n">    exponent. </param>
///
/// <returns>   (this matrix&lt;T&gt;) ^ n. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
const matrix<T> matrix<T>::power(int n) const
{
  if (rowN != colN)
  {
    return matrix<T>();
  }

  if (n == 0)
  {
    return matrix<T>(rowN, 1);
  }

  matrix<T> append;
  if (n < 0)
  {
    append = inverse();
    if (append.area() == 0)
    {
      cerr << "Power: matrix<T> is not invertible." << endl;
      return matrix<T>();
    }
  }
  else
  {
    append = *this;
  }

  matrix<T> result(rowN, 1);
  for (unsigned int i = abs(n); i; i >>= 1)
  {
    if (i & 1)
    {
      result *= append;
    }
    append *= append;
  }
  return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Additional member function. </summary>
///
/// <returns>   rowN * colN. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline int matrix<T>::area() const
{
  return rowN * colN;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   get the matrix&lt;T&gt; size; m and n become rowN and colN, respectively. </summary>
///
/// <param name="r">    [in,out] int reference to receive row size. </param>
/// <param name="c">    [in,out] int reference to receive column size. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
tuple<int, int> matrix<T>::size() const
{
  return tuple<int, int>(rowN, colN);
}

template <typename T>
matrix<T> matrix<T>::rand(int r, int c)
{
  matrix<T> result(r, c);
  bernoulli_distribution dis(0.5);

  T* end = result.elements + r * c;
  for (T* p = result.elements; p < end; p++)
  {
    *p = dis(engine);
  }
  return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Return whether or not m is the identity matrix&lt;T&gt;. </summary>
///
/// <param name="m">    The const matrix&lt;T&gt;&amp; to determine. </param>
///
/// <returns>   true if identity, false if not. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
bool matrix<T>::is_identity() const
{
  if (rowN != colN)
    return false;

  auto it = elements.begin();
  int jump = colN + 1;
  for (int i = 0; i < elements.area(); i++)
    if (elements[i] != !(i % jump))
      return false;

  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Return whether or not zero matrix&lt;T&gt;. </summary>
///
/// <returns>   true if zero matrix&lt;T&gt;, false if not. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
bool matrix<T>::is_zero() const
{
  return any_of(elements.begin(), elements.end(), [](T& x) { return x != 0; });
}

template <typename T>
void matrix<T>::reserve(int row, int col) {
  rowN = row;
  colN = col;
  elements.clear();
  elements.reserve(row * col);
}
// Operators

template <typename T>
matrix<T> operator +(const matrix<T>& a, const matrix<T>& b)
{
  if (a.rowN != b.rowN || a.colN != b.colN)
  {
    cerr << "ASSERT: matrix<T> size is not equal." << endl;
    return matrix<T>();
  }

  matrix<T> result;
  result.reserve(a.rowN, a.colN);

  transform(begin(a.elements), end(a.elements), begin(b.elements),
    back_inserter(result.elements), plus<T>());

  return result;
}

template <typename T>
matrix<T> operator -(const matrix<T>& a, const matrix<T>& b)
{
  if (a.rowN != b.rowN || a.colN != b.colN)
  {
    cerr << "ASSERT: matrix<T> size is not equal." << endl;
    return matrix<T>();
  }

  matrix<T> result;
  result.reserve(a.rowN, a.colN);

  transform(a.elements.begin(), a.elements.end(),
    b.elements.begin(), back_inserter(result.elements),
    minus<T>());

  return result;
}

template <typename T>
matrix<T> operator *(const matrix<T>& a, const matrix<T>& b)
{
  if (a.colN != b.rowN)
  {
    cerr << "Multiplication: colN and rowN not equal." << endl;
    return matrix<T>();
  }

  matrix<T> result;
  result.reserve(a.rowN, b.colN);
  vector<T>& elems = result.elements;
  matrix<T> transposed{ b.transpose() };

  for (int i = 0; i < a.rowN; i++)
  {
    const matrix_row<T> row = a[i];
    for (int j = 0; j < b.colN; j++)
    {
      elems.push_back(inner_product(
        row.begin(), row.end(), transposed[j].begin(), T()));
    }
  }

  return result;
}

template <typename T>
matrix<T> operator /(const matrix<T>& a, const matrix<T>& b)
{
  if (a.rowN != b.colN || a.colN != b.rowN)
  {
    cerr << "Division: colN and rowN not equal." << endl;
    return matrix<T>();
  }

  return a * b.inverse();
}

template <typename T>
matrix<T> get_wide_matrix(const matrix<T>& a) {
  int r, c;
  tie(r, c) = a.size();
  if (!(r == 1 && c == 3)) {
    if (r == 3 && c == 1) {
      return a.transpose();
    }
    else {
      return matrix<T>{};
    }
  }
  else return matrix<T>{a};
}

template <typename T>
matrix<T> cross_product(const matrix<T>& a, const matrix<T>& b) {
  matrix<T> wa = get_wide_matrix(a);
  matrix<T> wb = get_wide_matrix(b);
  if (wa.area() == 0 || wb.area() == 0) {
    cerr << "cross product is defined only for 3 dimension vector" << endl;
    return wa;
  }

  auto& ra = wa[0];
  auto& rb = wb[0];
  T e1 = ra[1] * rb[2] - ra[2] * rb[1];
  T e2 = ra[2] * rb[0] - ra[0] * rb[2];
  T e3 = ra[0] * rb[1] - ra[1] * rb[0];

  return { {e1, e2, e3} };
}

template <typename T>
matrix<T> operator %(const matrix<T>& a, const matrix<T>& b) {
  return cross_product(a, b);
}

template <typename T, typename K>
matrix<T> operator *(const K& coef, const matrix<T>& mat) {
  matrix<T> result;
  result.reserve(mat.rowN, mat.colN);

  transform(
    mat.elements.begin(), mat.elements.end(),
    back_inserter(result.elements),
    [&](const T& e) { return coef * e; });

  return result;
}

template <typename T, typename K>
matrix<T> operator *(const matrix<T>& mat, const K& coef) {
  return coef * mat;
}

template <typename T>
template <typename K>
matrix<T> matrix<T>::operator /(const K& divisor) const {
  matrix<T> div;
  div.reserve(rowN, colN);
  auto beg = elements.begin();
  auto end = elements.end();
  auto ins = back_inserter(div.elements);
  auto dvd = [&](const T& a) { return a / divisor; };
  transform(beg, end, ins, dvd);
  return div;
}

template <typename T>
matrix<T> matrix<T>::operator -() const
{
  matrix<T> result(*this);
  vector<T>& elems = result.elements;
  transform(begin(elems), end(elems), begin(elems), negate<T>());
  return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   matrix&lt;T&gt; indexer operator which doesn't range check. </summary>
///
/// <param name="i">    Index of row. </param>
///
/// <returns>   The pointer of the row's 1st T element. </returns>
///
/// <example>   (*this)[3][3] = true; </example>
////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline matrix_row<T> matrix<T>::operator [](int i)
{
  auto beg = elements.begin() + i * colN;
  auto end = beg + colN;
  return matrix_row<T>(beg, end);
}

template <typename T>
inline const matrix_row<T> matrix<T>::operator [](int i) const
{
  auto beg = elements.cbegin() + i * colN;
  auto end = beg + colN;
  return matrix_row<T>(beg, end);
}

template <typename T>
matrix<T>& matrix<T>::operator =(const matrix<T>& other)
{
  if (this == &other)
    return *this;

  rowN = other.rowN;
  colN = other.colN;
  elements = other.elements;
  return *this;
}

template <typename T>
matrix<T>& matrix<T>::operator =(matrix<T> && other)
{
  rowN = other.rowN;
  colN = other.colN;
  swap(elements, other.elements);

  return *this;
}

template <typename T>
matrix<T>& matrix<T>::operator +=(const matrix<T>& other)
{
  return *this = *this + other;
}

template <typename T>
matrix<T>& matrix<T>::operator -=(const matrix<T>& other)
{
  return *this = *this - other;
}

template <typename T>
matrix<T>& matrix<T>::operator *=(const matrix<T>& other)
{
  return *this = *this * other;
}

template <typename T>
matrix<T>& matrix<T>::operator /=(const matrix<T>& other)
{
  return *this = *this / other;
}

template <typename T>
T operator ==(const matrix<T>& a, const matrix<T>& b)
{
  return a.size() == b.size() &&
    equal(begin(a.elements), end(a.elements), begin(b.elements));
}

template <typename T>
T operator !=(const matrix<T>& a, const matrix<T>& b)
{
  return !(a == b);
}

template <typename T>
T operator < (const matrix<T>& a, const matrix<T>& b)
{
  if (a.area() > b.area())
    return false;

  return a.area() < b.area() ||
    a.area() == b.area() && a.rowN < b.rowN ||
    lexicographical_compare(
      begin(a.elements), end(a.elements),
      begin(b.elements), end(b.elements));
}

template <typename T>
T operator <=(const matrix<T>& a, const matrix<T>& b)
{
  return (a < b || a == b);
}

template <typename T>
T operator > (const matrix<T>& a, const matrix<T>& b)
{
  return !(a <= b);
}

template <typename T>
T operator >=(const matrix<T>& a, const matrix<T>& b)
{
  return !(a < b);
}

template <typename T>
string matrix<T>::str() const {
  return (stringstream() << *this).str();
}

template <typename T>
matrix<T> matrix<T>::fromStr(const string& src) {
  matrix<T> mat;
  stringstream(src) >> mat;
  return mat;
}

template <typename T>
ostream& operator <<(ostream& out, const matrix<T>& m)
{
  auto it = m.elements.begin();
  out << '(';
  if (m.rowN) out << m[0];
  for (int i = 1; i < m.rowN; i++)
  {
    cout << ", ";
    out << m[i];
  }
  cout << ')';

  return out;
}

template <typename T>
istream& operator >>(istream& in, matrix<T>& mat)
{
  auto is_row_delim = [](char ch) {
    return ch == -1 || ch == ')' || ch == '\n';
  };
  auto is_no_elem_in_row = [&]() {
    while (true) {
      int ch = in.peek();
      if (isspace(ch) || ch == ')') in.get();
      else return is_row_delim(ch);

      if (is_row_delim(ch)) return true;
    }
  };
  auto next_col = [&](T& e) {
    while (true) {
      if (is_no_elem_in_row()) return false;
      if (in >> e) return true;
      in.clear();
      char ch = in.get();
      ch = ch;
    }
  };

  auto& elems = mat.elements;
  elems.clear();

  // Ignore leading whitespaces
  in >> ws;

  // Get the first row column count
  for (T e; next_col(e); elems.push_back(e));
  mat.colN = static_cast<int>(elems.size());

  // Get remaining elements
  while (true) {
    T e;
    int count = 0;
    while (next_col(e)) {
      elems.push_back(e);
      count++;
    }
    if (count == 0) break;
  }
  mat.rowN = static_cast<int>(elems.size() / mat.colN);

  return in;
}
