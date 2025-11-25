#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_TRIPS 100       
#define MAX_STRING 100      

// Struct Location: Dung chung cho Diem di va Diem den
typedef struct {
    char name[MAX_STRING];      // Ten tinh/thanh pho
    char address[MAX_STRING];   // Dia chi chi tiet
} Location;

// Struct Trip: Quan ly thong tin chuyen xe
typedef struct {
    char tripId[MAX_STRING];    // Ma chuyen xe (ID)
    Location departure;         // Diem di (Struct long struct - Nested Struct)
    Location destination;       // Diem den (Struct long struct)
    char date[MAX_STRING];      // Ngay gio khoi hanh
    int totalSeats;             // Tong so ghe
    int bookedSeats;            // So ghe da dat
} Trip;

// Bien toan cuc (Global variables) de luu tru du lieu
Trip tripList[MAX_TRIPS];   // Mang luu danh sach chuyen xe
int currentTripCount = 0;   // Bien dem so luong chuyen xe hien co

void removeNewline(char* str);              // Xoa ky tu xuong dong thua
int isEmptyOrSpace(char* str);              // Kiem tra chuoi rong
int isIdDuplicate(char* id);                // Kiem tra trung ID
void getValidString(char* prompt, char* outStr); // Nhap chuoi co validate
int getValidInt(char* prompt);              // Nhap so nguyen duong
int getMenuChoice();                        // Nhap lua chon menu an toan
void createNewTrip();                       // Chuc nang 1: Them chuyen xe
void updateTrip();                          // Chuc nang 2: Cap nhat chuyen xe
void viewAllTrips();                        // Chuc nang 3: Xem danh sach

// Day la noi chuong trinh bat dau chay
int main() {
    int choice; // Bien luu lua chon cua nguoi dung
    
    // Vong lap vo han while(1) giup Menu hien lai lien tuc sau khi xong lenh
    while (1) {
        printf("\n================ QUAN LY CHUYEN XE ================\n");
        printf("1. Them chuyen xe moi (Create New Trip)\n");
        printf("2. Cap nhat thong tin chuyen xe (Update Trip)\n");
        printf("3. Hien thi tat ca chuyen xe (View All)\n");
        printf("0. Thoat (Exit)\n");
        printf("===================================================\n");
        printf("Nhap lua chon cua ban: ");

        // Lay lua chon tu ban phim (xu ly chong troi lenh)
        choice = getMenuChoice();

        // Dieu huong chuong trinh theo lua chon
        switch (choice) {
            case 1:
                createNewTrip(); // Goi ham them moi
                break;
            case 2:
                updateTrip();    // Goi ham cap nhat
                break;
            case 3:
                viewAllTrips();  // Goi ham hien thi
                break;
            case 0:
                printf("\nDa thoat chuong trinh. Tam biet!\n");
                return 0; // Tra ve 0 bao hieu ket thuc thanh cong
            default:
                printf("\n[LOI] Lua chon khong hop le. Vui long nhap lai (0-3).\n");
        }
    }
    return 0;
}

// 4. CHI TIET CAC HAM CHUC NANG 

// Chuc nang 1: Them chuyen xe moi (F01)
void createNewTrip() {
    // Kiem tra xem mang da day chua
    if (currentTripCount >= MAX_TRIPS) {
        printf("\n[LOI] Danh sach da day (Max 100). Khong the them moi!\n");
        return;
    }

    Trip newTrip; // Tao bien tam de nhap du lieu
    printf("\n--- [F01] THEM CHUYEN XE MOI ---\n");

    // Vong lap kiem tra trung ID (Yeu cau Validation 1)
    do {
        getValidString("Nhap Ma chuyen xe (Trip ID): ", newTrip.tripId);
        // Neu ham isIdDuplicate tra ve 1 (trung) -> Bao loi
        if (isIdDuplicate(newTrip.tripId)) {
            printf(" -> [LOI] Ma chuyen xe nay DA TON TAI. Vui long chon ma khac!\n");
        } else {
            break; // ID hop le, thoat vong lap
        }
    } while (1);

    // Nhap thong tin Diem Di (Yeu cau Validation 2)
    printf("-- Thong tin Diem Di --\n");
    getValidString("Nhap Ten tinh/TP di: ", newTrip.departure.name);
    getValidString("Nhap Dia chi chi tiet diem di: ", newTrip.departure.address);

    // Nhap thong tin Diem Den
    printf("-- Thong tin Diem Den --\n");
    getValidString("Nhap Ten tinh/TP den: ", newTrip.destination.name);
    getValidString("Nhap Dia chi chi tiet diem den: ", newTrip.destination.address);

    // Nhap Ngay gio (Yeu cau Validation 4)
    getValidString("Nhap Ngay gio khoi hanh (VD: 20/11/2024 08:00): ", newTrip.date);

    // Nhap Tong so ghe (Yeu cau Validation 3: phai > 0)
    newTrip.totalSeats = getValidInt("Nhap Tong so ghe (Total Seats): ");

    // Khoi tao so ghe da dat = 0 nhu yeu cau de bai
    newTrip.bookedSeats = 0;

    // Luu du lieu vao mang va tang bien dem len 1
    tripList[currentTripCount] = newTrip;
    currentTripCount++;

    printf("\n[THANH CONG] Da them chuyen xe moi!\n");
}

// Chuc nang 2: Cap nhat thong tin chuyen xe (F02)
void updateTrip() {
    char searchId[MAX_STRING];
    int foundIndex = -1; // -1 nghia la chua tim thay

    printf("\n--- [F02] CAP NHAT THONG TIN CHUYEN XE ---\n");
    getValidString("Nhap Ma chuyen xe can cap nhat: ", searchId);

    // Duyet mang de tim vi tri chuyen xe can sua
    for (int i = 0; i < currentTripCount; i++) {
        if (strcmp(tripList[i].tripId, searchId) == 0) {
            foundIndex = i; // Luu lai vi tri tim thay
            break;
        }
    }

    // Neu khong tim thay (foundIndex van la -1)
    if (foundIndex == -1) {
        printf(" -> [LOI] Khong tim thay chuyen xe co ma: %s\n", searchId);
        return;
    }

    // Neu tim thay, bat dau nhap du lieu moi
    printf("\nTIM THAY! Vui long nhap thong tin moi (ID giu nguyen):\n");
    Trip tempTrip = tripList[foundIndex]; // Lay du lieu cu ra de sua

    printf("-- Cap nhat Diem Di --\n");
    getValidString("Nhap Ten tinh/TP di moi: ", tempTrip.departure.name);
    getValidString("Nhap Dia chi diem di moi: ", tempTrip.departure.address);

    printf("-- Cap nhat Diem Den --\n");
    getValidString("Nhap Ten tinh/TP den moi: ", tempTrip.destination.name);
    getValidString("Nhap Dia chi diem den moi: ", tempTrip.destination.address);

    getValidString("Nhap Ngay gio khoi hanh moi: ", tempTrip.date);

    // Validate TotalSeats: Khong duoc nho hon so ghe da dat hien tai
    do {
        tempTrip.totalSeats = getValidInt("Nhap Tong so ghe moi: ");
        if (tempTrip.totalSeats < tripList[foundIndex].bookedSeats) {
            printf(" -> [LOI] Tong ghe (%d) khong duoc nho hon so ghe da dat (%d)!\n", 
                   tempTrip.totalSeats, tripList[foundIndex].bookedSeats);
        } else {
            break;
        }
    } while (1);

    // Gan nguoc lai vao danh sach
    tripList[foundIndex] = tempTrip;
    printf("\n[THANH CONG] Cap nhat thong tin chuyen xe hoan tat!\n");
}

// Chuc nang 3: Hien thi danh sach
void viewAllTrips() {
    printf("\n--- DANH SACH CHUYEN XE ---\n");
    if (currentTripCount == 0) {
        printf("Danh sach hien tai dang trong.\n");
    } else {
        for(int i = 0; i < currentTripCount; i++) {
            printf("%d. ID: %s | %s -> %s | Ghe: %d/%d\n", 
                   i+1, tripList[i].tripId, 
                   tripList[i].departure.name, tripList[i].destination.name,
                   tripList[i].bookedSeats, tripList[i].totalSeats);
        }
    }
}

// --- 5. DINH NGHIA CAC HAM TIEN ICH (UTILITIES) ---

// Xoa ky tu \n o cuoi chuoi do fgets tao ra
void removeNewline(char* str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') str[len - 1] = '\0';
}

// Kiem tra chuoi co rong khong (Validate Input)
int isEmptyOrSpace(char* str) {
    if (strlen(str) == 0) return 1; // Rong tuyet doi
    // Kiem tra neu chi toan dau cach
    for (int i = 0; i < strlen(str); i++) {
        if (!isspace(str[i])) return 0; // Co ky tu khac dau cach -> OK
    }
    return 1; // Chi toan dau cach -> Loi
}

// Kiem tra trung ID trong toan bo danh sach
int isIdDuplicate(char* id) {
    for (int i = 0; i < currentTripCount; i++) {
        if (strcmp(tripList[i].tripId, id) == 0) 
        
		return 1; 
    }
    return 0; 
}

// Ham nhap chuoi an toan: Bat buoc nhap, khong duoc de trong
void getValidString(char* prompt, char* outStr) {
    do {
        printf("%s", prompt);
        fgets(outStr, MAX_STRING, stdin); // Dung fgets thay scanf de nhan khoang trang
        removeNewline(outStr);
        if (isEmptyOrSpace(outStr)) {
            printf(" -> [LOI] Thong tin nay khong duoc de TRONG. Vui long nhap lai!\n");
        } else break;
    } while (1);
}

// Ham nhap so nguyen an toan: Phai la so va > 0
int getValidInt(char* prompt) {
    int value;
    char buffer[50];
    do {
        printf("%s", prompt);
        fgets(buffer, 50, stdin);
        // sscanf giup chuyen chuoi thanh so, tranh loi troi lenh
        if (sscanf(buffer, "%d", &value) == 1 && value > 0) return value;
        printf(" -> [LOI] Vui long nhap mot so nguyen duong hop le (> 0)!\n");
    } while (1);
}

// Ham lay lua chon menu an toan (tranh nhap chu cai lam treo chuong trinh)
int getMenuChoice() {
    int value;
    char buffer[50];
    if (fgets(buffer, 50, stdin) != NULL) {
        if (sscanf(buffer, "%d", &value) == 1) return value;
    }
    return -1; // Tra ve -1 neu nhap sai dinh dang
}
