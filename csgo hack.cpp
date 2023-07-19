#include <Windows.h>
#include <iostream>
#include <math.h>
#include <TlHelp32.h>
#include <array>

using namespace std;

const int entitiesLength = 24;

DWORD pid;
DWORD playerBase;    //client.dll
DWORD enginedll;     //contains writeable viewing angle

DWORD moduleBase;
DWORD entities[entitiesLength];
DWORD entity;
HANDLE pHandle;
DWORD entityTarget;
bool isActive = false;
bool isStartingElm = true;
int startingElm;
int elmNum;
int dormant;
int playerCrouch;
int enemyCrouch;

struct {
    DWORD health = 0x100;
    DWORD horizontal = 0x0130;
    DWORD vertical = 0x012C;
    DWORD x = 0x450;
    DWORD y = 0x470;
    DWORD z = 0x460;
    DWORD armorValue = 0x117CC;
    DWORD glowIndex = 0x10488;
    DWORD viewAngle = 0x4D90;
    DWORD writeHorizontal = 0x4D94;
    DWORD writeVertical = 0x4D90;
    DWORD localPlayer = 0xDE997C;
    DWORD bot = 0xAAD58C;
    DWORD engine = 0x59F19C;
    DWORD entityList = 0x4DFEF0C;
    DWORD isDormant = 0xED;
    DWORD panorama = 0x25840C;
    DWORD crouchCheck = 0x104;
    
} offset;


const double pi = atan(1)*4;

float player_x, player_y, player_z, enemy_x, enemy_y, enemy_z, test_enm_x, test_enm_y, test_enm_z, test_delta_x, test_delta_y, test_delta_z, test_displacement, test_displacement_3d;
float delta_x, delta_y, delta_z, displacement, displacement_3d, finalHorizontalAngle, finalVerticalAngle, viewing_x, viewing_y, enm_viewing_x;

long enemyHealth;

DWORD GetModuleBaseAddress(const char* modName) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry)) {
            do {
                if (!strcmp(modEntry.szModule, modName)) {
                    CloseHandle(hSnap);
                    return (DWORD)modEntry.modBaseAddr;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
}



template <typename T> T RPM(DWORD baseAddr, DWORD ptrOffset, T buffer) {
    ReadProcessMemory(pHandle, (LPCVOID)(baseAddr + ptrOffset), &buffer, sizeof(buffer), NULL);
    return buffer;
}

template <typename T> void WPM(DWORD baseAddr, DWORD ptrOffset, T buffer) {
    WriteProcessMemory(pHandle, (void*)(baseAddr + ptrOffset), &buffer, sizeof(buffer), NULL);
}

int main()

{
    HWND hWnd = FindWindowA(0, ("Counter-Strike: Global Offensive - Direct3D 9"));
    cout << "hWnd : " << hWnd << "\n";
    GetWindowThreadProcessId(hWnd, &pid);
    pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    playerBase = RPM(GetModuleBaseAddress("client.dll"), offset.localPlayer, playerBase);
    enginedll = RPM(GetModuleBaseAddress("engine.dll"), offset.engine, enginedll);
    cout << "client.dll : " << hex << playerBase << dec << "\n";
    

    while (hWnd != NULL) {
        if (GetAsyncKeyState('T') & 0x8000) {
            isActive = !isActive;
            while (GetAsyncKeyState('T') & 0x8000) {}
        }
        if (isActive) {
            player_x = RPM(playerBase, offset.x, player_x);
            playerCrouch = RPM(playerBase, offset.crouchCheck, playerCrouch);
            if (playerCrouch > 257) {
                player_y = RPM(playerBase, offset.y, player_y) - 20;
            }
            else {
                player_y = RPM(playerBase, offset.y, player_y);
            }
            
            player_z = RPM(playerBase, offset.z, player_z);
            
            viewing_x = RPM(playerBase, offset.horizontal, viewing_x);
            viewing_y = RPM(playerBase, offset.vertical, viewing_y);



            

            for (int i = 1; i < entitiesLength; i++) {
                entity = RPM(GetModuleBaseAddress("client.dll"), offset.entityList + i * 0x10, entity);
                //cout << entity << "\n";
                if (entity != NULL) {
                    entities[elmNum] = entity;
                    elmNum += 1;
                }
            }
            isStartingElm = true;
            for (int i = 0; i < entitiesLength; i++) {
                if ((entities[i] != NULL) && (RPM(entities[i], offset.isDormant, dormant) == 0)&& (RPM(entities[i], offset.health, enemyHealth) > 0)) {
                    test_enm_x = RPM(entities[i], offset.x, test_enm_x);
                    enemyCrouch = RPM(entities[i], offset.crouchCheck, enemyCrouch);
                    //cout << "crouch : " << enemyCrouch << "\n\n";
                    if (enemyCrouch > 769) {
                        test_enm_y = RPM(entities[i], offset.y, test_enm_y) - 20;
                    }
                    else {
                        test_enm_y = RPM(entities[i], offset.y, test_enm_y) + 2;
                    }
                    test_enm_z = RPM(entities[i], offset.z, test_enm_z);

                    enm_viewing_x = RPM(entities[i], offset.horizontal, enm_viewing_x);
                    //cout << enm_viewing_x << "  +  x1: ";
                    //cout << test_enm_x << "   z1: " << test_enm_z << "  -  ";
                    
                    test_enm_x += cos(enm_viewing_x * pi / 180) * 10;
                    test_enm_z += sin(enm_viewing_x * pi / 180) * 10;
                    cout << cos(enm_viewing_x) * 10 << "   ";
                    cout << sin(enm_viewing_x) * 10 << "\n";

                    test_delta_x = abs(player_x - test_enm_x);
                    test_delta_y = player_y - test_enm_y;
                    test_delta_z = abs(player_z - test_enm_z);

                    test_displacement = sqrt(pow(test_delta_x, 2) + pow(test_delta_z, 2));
                    test_displacement_3d = sqrt(pow(test_delta_x, 2) + pow(test_delta_z, 2) + pow(test_delta_y, 2));
                    cout << "entity " << i << " = " << test_displacement << "\n";
                    if (displacement_3d == 0) {
                        displacement = sqrt(pow(test_delta_x, 2) + pow(test_delta_z, 2));
                        displacement_3d = sqrt(pow(test_delta_x, 2) + pow(test_delta_z, 2) + pow(test_delta_y, 2));
                        enemy_x = test_enm_x;
                        enemy_y = test_enm_y;
                        enemy_z = test_enm_z;

                        delta_x = test_delta_x;
                        delta_y = test_delta_y;
                        delta_z = test_delta_z;
                        entityTarget = entities[i];
                    }
                    else {
                        if ((test_displacement_3d < displacement_3d) && (test_displacement_3d != 0)) {
                            displacement = test_displacement;
                            displacement_3d = test_displacement_3d;
                            enemy_x = test_enm_x;
                            enemy_y = test_enm_y;
                            enemy_z = test_enm_z;

                            delta_x = test_delta_x;
                            delta_y = test_delta_y;
                            delta_z = test_delta_z;
                            entityTarget = entities[i];
                        }
                    }
                }
                //else { break; }
            }

            
            finalHorizontalAngle = (atan(delta_x / delta_z) * 180 / pi) + 90;
            finalVerticalAngle = (atan(delta_y / displacement) * 180) / pi;

            if ((player_x > enemy_x) && (player_z < enemy_z)) {
                finalHorizontalAngle = finalHorizontalAngle;
            }
            else if ((player_x > enemy_x) && (player_z > enemy_z)) {
                finalHorizontalAngle = 360 - finalHorizontalAngle;
            }
            else if ((player_x < enemy_x) && (player_z > enemy_z)) {
                finalHorizontalAngle = finalHorizontalAngle - 180;
            }
            else if ((player_x < enemy_x) && (player_z < enemy_z)) {
                finalHorizontalAngle = 180 - finalHorizontalAngle;
            }
            if (displacement != 0) {
                WPM(enginedll, offset.writeHorizontal, finalHorizontalAngle);
                WPM(enginedll, offset.writeVertical, finalVerticalAngle);
            }
            
            cout << "Distance : " << displacement_3d << "  x : " << finalHorizontalAngle << "  y : " << finalVerticalAngle << "  Target : " << hex << entityTarget << dec << "\n";
            displacement = 0;
            displacement_3d = 0;
            enemy_x = 0;
            enemy_y = 0;
            enemy_z = 0;

            delta_x = 0;
            delta_y = 0;
            delta_z = 0;
            entityTarget = 0;
            elmNum = 0;
            enemyHealth = 0;
            for (int i = 0; i < entitiesLength; i++) {
                entities[i] = 0;
            }

            //Sleep(50);
        }
        
    }
    return 0;
}
