# AzTank Trouble

## Mô tả dự án
AzTank Trouble là một game bắn súng arcade theo phong cách AzTank Trouble, được xây dựng bằng SDL2 (C++). Game có các tính năng:
- **Đối tượng chính:** Xe tăng (có thể là 1 hoặc nhiều xe) đại diện cho người chơi.
- **Input bàn phím:** Hỗ trợ 2 người chơi (mũi tên và WASD) với khả năng điều khiển riêng biệt.
- **Tương tác đối tượng:** Xử lý va chạm giữa đạn, xe tăng và các tường với hiệu ứng phản xạ.
- **Hiển thị điểm số:** Sử dụng SDL_ttf để render text.
- **Chế độ chơi:** Player vs Player và Player vs Computer (với AI đơn giản).
- **Ngoại lực:** Ví dụ như “gió” tác động lên đạn, làm thay đổi hướng di chuyển.

## Cài đặt thư viện
Vì tôi dùng MSYS MINGW64 nên cài thông qua nó luôn. Mở giao diện MSYS MINGW64 lên và gõ từng lệnh sau:
```basg
pacman -Syu

pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer
```
## Lệnh chạy chương trình
```basg
.\build.bat

.\AzTankTrouble.exe
```


## Cấu trúc dự án
```plaintext
AzTankTrouble/
 ├── README.md # Hướng dẫn dự án, cấu trúc và thứ tự code 
 ├── CMakeLists.txt # (hoặc Makefile) để build dự án 
 ├── assets/ # Thư mục chứa tài nguyên 
 │  ├── images/ 
 │  ├── sounds/ 
 │  └── fonts/ 
 ├── src/ # Mã nguồn của game 
 │  ├── main.cpp # Hàm main, khởi tạo SDL, cửa sổ, vòng lặp chính 
 │  ├── Game.h / Game.cpp # Lớp quản lý game: vòng lặp, cập nhật, render 
 │  ├── Tank.h / Tank.cpp # Lớp xe tăng: thuộc tính, di chuyển, xử lý input 
 │  ├── Bullet.h / Bullet.cpp # Lớp đạn: chuyển động, va chạm, phản xạ 
 │  ├── AI.h / AI.cpp # Lớp AI cho chế độ Player vs Computer 
 │  ├── Maze.h / Maze.cpp # Vẽ mê cung
 │  └── UI.cpp

 
```


## Thứ tự phát triển (Coding Order)
1. **Thiết lập môi trường & cấu hình dự án:**
   - Cài đặt và cấu hình SDL2, SDL_image, SDL_ttf.
   - Tạo file build (CMakeLists.txt hoặc Makefile) để biên dịch dự án.

2. **Khởi tạo SDL và tạo cửa sổ game (main.cpp):**
   - Khởi tạo SDL, tạo cửa sổ và renderer.
   - Tạo vòng lặp sự kiện cơ bản để kiểm tra việc khởi chạy.

3. **Xây dựng lớp quản lý game (Game):**
   - Tạo `Game.h` và `Game.cpp` để quản lý vòng lặp chính, cập nhật logic game và render.
   - Tích hợp các thành phần (tank, đạn, UI) vào vòng lặp game.

4. **Phát triển đối tượng chính: Xe tăng (Tank):**
   - Tạo `Tank.h` và `Tank.cpp` để định nghĩa thuộc tính (vị trí, tốc độ, hướng) và phương thức (move, render, xử lý input) của xe tăng.
   - Xử lý input bàn phím riêng cho từng xe tăng (ví dụ: mũi tên cho P1, WASD cho P2).

5. **Phát triển đối tượng đạn (Bullet):**
   - Tạo `Bullet.h` và `Bullet.cpp` để xử lý chuyển động, trạng thái sống của đạn.
   - Tích hợp chức năng bắn đạn từ xe tăng.

6. **Xử lý va chạm và phản xạ (Collision Manager):**
   - Tạo `CollisionManager.h` và `CollisionManager.cpp` để kiểm tra va chạm giữa:
     - Đạn và tường (với hiệu ứng phản xạ).
     - Đạn và xe tăng (để xác định trúng, gây hại, v.v.).
   - Thực hiện tính toán đơn giản cho phản xạ (ví dụ: đổi hướng vector khi chạm biên).

7. **Tích hợp giao diện người dùng và điểm số:**
   - Sử dụng SDL_ttf để render điểm số và các thông báo (ví dụ: “Game Over”, “Score”).
   - Tạo lớp hoặc module riêng để quản lý UI và cập nhật điểm số.

8. **Phát triển chế độ chơi đa người:**
   - Cấu hình input cho 2 người chơi: phân biệt giữa các phím điều khiển (mũi tên và WASD).
   - Đảm bảo mỗi xe tăng phản ứng đúng với các phím của mình.

9. **Tạo chế độ Player vs Computer (AI):**
   - Tạo `AI.h` và `AI.cpp` để xây dựng logic cho xe tăng điều khiển bởi máy.
   - Lập trình AI đơn giản: theo dõi vị trí của người chơi, di chuyển tránh va chạm và bắn đạn khi có cơ hội.

10. **Thêm hiệu ứng ngoại lực và chi tiết phụ:**
    - Cài đặt các hiệu ứng ngoại lực (như “gió”) tác động lên đạn.
    - Tinh chỉnh hiệu ứng phản xạ, âm thanh, và hiệu ứng hình ảnh.

11. **Kiểm thử, debug và tối ưu hóa:**
    - Chạy thử từng phần của game sau khi hoàn thành chức năng.
    - Kiểm tra tính ổn định, hiệu suất và trải nghiệm người chơi.
    - Sửa lỗi và tối ưu code theo từng module.

12. **Đóng gói và hoàn thiện:** (hên xui)
    - Kiểm tra lại toàn bộ chức năng của game.
    - Chuẩn bị tài nguyên và cấu hình cho phiên bản phát hành.

## Lời khuyên
- **Phát triển theo từng bước:** Bắt đầu từ khung cơ bản (SDL, cửa sổ, vòng lặp) rồi dần dần thêm tính năng.
- **Kiểm thử liên tục:** Mỗi khi hoàn thành 1 module, hãy kiểm tra để đảm bảo không có lỗi.
- **Ghi chú trong code:** Sử dụng comments để giải thích các đoạn code phức tạp giúp dễ bảo trì sau này.
- **Tài liệu tham khảo:** Tham khảo các tutorial về SDL2 như [Lazy Foo’s SDL Tutorial](http://lazyfoo.net/tutorials/SDL/) để hiểu rõ hơn về cách làm việc với SDL.

Chúc bạn thành công với dự án AzTank Trouble!
