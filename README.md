b1: vào build và điểu chỉnh vis.mac cho phù hợp với yêu cầu bài toán
b2: mở terminal
b3: dùng lệnh "ccmake .."
b4: nhấn "c" để configure, nhấn "e" thoát và nhấn "g" để genergate
b5: nhập make để biên dịch sẽ tạo ra file wls.exe
b6: nhập "./wls" để chạy chương trình
b7: khi chương trình đã chạy dùng command "/run/beamOn [số sự kiện] " để chạy mô phỏng

lưu ý:
- để chạy được chương trình thì cần tải cmake cho geant4.
- các settup như nguồn phát, kích thước vật liệu có thể settup trong file vis.mac nằm trong build.
- mã nguồn đã được viết để settup trực tiếp trong file vis.mac.
- sau khi chạy sẽ được file wls.root, sửa tên file và chạy theo b1 cho các vị trí khác.
  ----Tinsh----
