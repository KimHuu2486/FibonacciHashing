# Fibonacci Hashing

## Mô tả
Dự án này triển khai **Fibonacci Hashing** sử dụng kĩ thuật **Separate Chaining** để xử lí **Collisions**. Đồng thời cũng triển khai **Modulo Hashing** để so sánh hiệu năng (thời gian thực thi, bộ nhớ sử dụng, độ dài chuỗi tối đa...).

## Tính năng
- Đọc dữ liệu từ file văn bản.
- Loại bỏ các khóa trùng lặp.
- Sử dụng kĩ thuật multiplication, modulo, bit operation để băm dữ liệu.
- Xử lí collisions bằng phương pháp Separate Chaining.
- Tìm kiếm từ khóa, chèn và xóa dữ liệu.
- Đo thời gian và bộ nhớ sử dụng để xây dựng bảng băm, tìm kiếm, chèn, xóa dữ liệu.

## Cài đặt

### Yêu cầu:
- Visual Studio 2022 hoặc trình biên dịch C++ hỗ trợ C++17 trở lên.

### Cách chạy:
1. Mở file `main.cpp` trong Visual Studio 2022 hoặc trình biên dịch có hỗ trợ C++.
2. Đảm bảo file dữ liệu đầu vào (ví dụ: `Test1_Random.txt`) có định dạng mỗi đoạn (không xuống dòng) là một chuỗi, trong đó từ khóa là từ đầu tiên.
3. Biên dịch và chạy chương trình.
4. Nhập đường dẫn đến file dữ liệu khi được yêu cầu.
5. Nhập số lượng từ khóa và từ khóa cần tra cứu hoặc xóa khi được yêu cầu.
5. Nhập số lượng dữ liệu và dữ liệu cần chèn khi được yêu cầu.


## Cấu trúc file

- `main.cpp`: Chứa toàn bộ mã nguồn của chương trình.
- `Data.zip`: File nén chứa dữ liệu đầu vào (các file .txt).
- `README.md`: Tài liệu mô tả dự án.


## Ví dụ dữ liệu

```txt
Apple  n. 1 roundish firm fruit with crisp flesh. 2 tree bearing this.  apple of one's eye cherished person or thing. [old english]
Banana  n. 1 long curved soft fruit with a yellow skin. 2 treelike plant bearing it.  go bananas slang go mad. [portuguese or spanish, from an african name]
Bike  colloq. —n. Bicycle or motor cycle. —v. (-king) ride a bike.  biker n. [abbreviation]
Bronze medal  n. Medal, usu. Awarded as third prize.
Dirty  —adj. (-ier, -iest) 1 soiled, unclean. 2 causing dirtiness (dirty job).
```

## Ví dụ cách chạy
```txt
Input the path of file: Test1_Random.txt

FIBONACCI HASHING:
Run time: 707ms         Memory: 43 MB
Load factor: 0.477554
Average chain length: 1.26371
Maximum chain length: 6

MODULO HASHING:
Run time: 714ms         Memory: 35 MB
Load factor: 0.477554
Average chain length: 1.2634
Maximum chain length: 5

------------------------------------------------------

Input the numbers of keys which you want to SEARCH: 1
Input 1 keys:
Apple

Result of Fibonacci Hashing:
Apple  n. 1 roundish firm fruit with crisp flesh. 2 tree bearing this.  apple of one's eye cherished person or thing. [old english]
Run time: 87 microseconds               Memory: 0 KB

Result of Modulo Hashing:
Apple  n. 1 roundish firm fruit with crisp flesh. 2 tree bearing this.  apple of one's eye cherished person or thing. [old english]
Run time: 106 microseconds             Memory: 0 KB

------------------------------------------------------

Input the numbers of keys which you want to DELETE: 1
Input 1 keys:
Apple

Result of Fibonacci Hashing:
Delete completed key Apple
Result of search key Apple after delete: Not Found
Run time: 225 microseconds              Memory: 0 KB

Result of Modulo Hashing:
Delete completed key Apple
Result of search key Apple after delete: Not Found
Run time: 179 microseconds              Memory: 0 KB

------------------------------------------------------

Input the numbers of keys and values which you want to INSERT: 1
Input 1 keys and values:
Apple n. my favorite fruit

Result of Fibonacci Hashing:
Insert key Apple completed
Result of search key Apple after insert:
Apple n. my favorite fruit
Run time: 178 microseconds             Memory: 0 KB

Result of Modulo Hashing:
Insert key Apple completed
Result of search key Apple after insert:
Apple n. my favorite fruit
Run time: 220 microseconds              Memory: 0 KB
```