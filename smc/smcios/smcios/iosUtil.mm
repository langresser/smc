//
//  iosUtil.m
//  smcios
//
//  Created by 王 佳 on 12-8-8.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "iosUtil.h"
#import "CGJoystick.h"
#import "video/uikit/SDL_uikitwindow.h"
#include "SDL_compat.h"
#include "core/game_core.h"
#include "core/global_game.h"
#include "overworld/world_player.h"
#include "level/level_player.h"
#include "input/keyboard.h"
#include "overworld/overworld.h"
#include "gui/menu.h"

#import "AdMoGoDelegateProtocol.h"
#import "AdMoGoView.h"
#import "AdMoGoWebBrowserControllerUserDelegate.h"

extern bool g_inFadingDraw;

bool isPad()
{
	BOOL result = NO;
	if ([[UIDevice currentDevice] respondsToSelector:@selector(userInterfaceIdiom)]) {
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 30200
		result = ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad);
#endif
	}
	return result;
}

void getScreenSize(int* width, int* height)
{
    CGSize size = [UIScreen mainScreen].bounds.size;
    if (width) {
        *width = size.width > size.height ? size.width : size.height;
    }
    
    if (height) {
        *height = size.width > size.height ? size.height : size.width;
    }
}

extern char g_application_dir[256];
extern char g_resource_dir[256];
void initDir()
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString  *plistPath = [paths objectAtIndex:0];
    strlcpy(g_resource_dir, [plistPath UTF8String], sizeof(g_resource_dir));
    
    g_resource_dir[strlen(g_resource_dir)] = '/';
}

void getFileStatus(const char* pszName)
{
    NSFileManager* fmgr = [NSFileManager defaultManager];
    if (!fmgr) {
        return;
    }
    
    NSError* error;
    NSDictionary* attr = [fmgr attributesOfItemAtPath:[NSString stringWithUTF8String:pszName] error:&error];
    
    
    NSLog(@"file: %s    attr:%@ ", pszName, attr);
}

#define JOYSTICK_NONE 0
#define JOYSTICK_MOVE 1
#define JOYSTICK_BATTLE 2
int g_joystickType = JOYSTICK_NONE;
bool g_adsIsShown = false;
@interface MyDelegate : NSObject<AdMoGoDelegate>
{
    CGJoystick* joystickBase;
    UIButton* btnJump;
    UIButton* btnFire;
    
    UIButton* btnMenu;
    UIButton* btnBack;
    
    AdMoGoView *adView;
}

@property (nonatomic, retain) AdMoGoView *adView;
@end

MyDelegate* g_delegate = nil;
@implementation MyDelegate
@synthesize adView;
+(MyDelegate*)sharedInstance
{
    if (g_delegate == nil) {
        g_delegate = [[MyDelegate alloc]init];
    }   
    
    return g_delegate;
}

- (UIViewController *)viewControllerForPresentingModalView {
    SDL_Window* window = SDL_GetWindowFromID(g_windowId);
    if (!window) {
        return nil;
    }
    
    SDL_WindowData* windowData = (SDL_WindowData*)window->driverdata;
    return windowData->viewcontroller;
}

-(void)showAds
{
    if (adView == nil) {
        adView = [[AdMoGoView alloc] initWithAppKey:@"c00f180a6c9e4f9289fc62daa2a10f3b"
                                             adType:AdViewTypeNormalBanner expressMode:NO
                                 adMoGoViewDelegate:self];
        int w, h;
        getScreenSize(&w, &h);
        adView.center = CGPointMake((w - 320)/ 2, (h - 50));

        SDL_Window* window = SDL_GetWindowFromID(g_windowId);
        if (!window) {
            return;
        }
        
        SDL_WindowData* windowData = (SDL_WindowData*)window->driverdata;
        [windowData->viewcontroller.view addSubview:adView];
    }
    
    if (adView.hidden == YES) {
        adView.hidden = NO;
    }
}

-(void)closeAds
{
    if (adView == nil) {
        return;
    }
    adView.hidden = YES;
}

-(void)showJoystick
{    
    if (joystickBase == nil) {
        int width, height;
        getScreenSize(&width, &height);
        
        if (isPad()) {
            joystickBase = [[CGJoystick alloc]initWithFrame:CGRectMake(30, height - 200 - 20, 200, 200)];
        } else {
            joystickBase = [[CGJoystick alloc]initWithFrame:CGRectMake(10, height - 150 - 10, 150, 150)];
        }
        
        SDL_Window* window = SDL_GetWindowFromID(g_windowId);
        if (!window) {
            return;
        }
        
        SDL_WindowData* windowData = (SDL_WindowData*)window->driverdata;
        UIView* mainView = windowData->viewcontroller.view;
        [mainView addSubview:joystickBase];
        
        int BUTTON_WIDTH = 80;
        if (!isPad()) {
            BUTTON_WIDTH = 50;
        }
        
        btnJump = [[UIButton alloc]initWithFrame:CGRectMake(width - BUTTON_WIDTH * 2.3, height - BUTTON_WIDTH * 1.3, BUTTON_WIDTH, BUTTON_WIDTH)];
        [btnJump setImage:[UIImage imageNamed:@"anormal"] forState:UIControlStateNormal];
        [btnJump setImage:[UIImage imageNamed:@"aclick"] forState:UIControlStateHighlighted];
        [btnJump addTarget:self action:@selector(onClickFire) forControlEvents:UIControlEventTouchDown];
        [btnJump addTarget:self action:@selector(onClickFireUp) forControlEvents:UIControlEventTouchUpInside];
        [btnJump addTarget:self action:@selector(onClickFireUp) forControlEvents:UIControlEventTouchUpOutside];
        [mainView addSubview:btnJump];
        btnJump.alpha = 0.5;
        
        btnFire = [[UIButton alloc]initWithFrame:CGRectMake(width - BUTTON_WIDTH * 1.1, height - BUTTON_WIDTH * 1.3, BUTTON_WIDTH, BUTTON_WIDTH)];
        [btnFire setImage:[UIImage imageNamed:@"bnormal"] forState:UIControlStateNormal];
        [btnFire setImage:[UIImage imageNamed:@"bclick"] forState:UIControlStateHighlighted];
        [btnFire addTarget:self action:@selector(onJumpDown) forControlEvents:UIControlEventTouchDown];
        [btnFire addTarget:self action:@selector(onJumpUp) forControlEvents:UIControlEventTouchUpInside];
        [btnFire addTarget:self action:@selector(onJumpUp) forControlEvents:UIControlEventTouchUpOutside];
        btnFire.alpha = 0.5;
        [mainView addSubview:btnFire];
        
        int MENU_BUTTON_WIDTH = 60;
        if (!isPad()) {
            MENU_BUTTON_WIDTH = 40;
        }
        btnMenu = [[UIButton alloc]initWithFrame:CGRectMake(2, 2, MENU_BUTTON_WIDTH, MENU_BUTTON_WIDTH)];
        [btnMenu setImage:[UIImage imageNamed:@"menunormal"] forState:UIControlStateNormal];
        [btnMenu setImage:[UIImage imageNamed:@"menuclick"] forState:UIControlStateHighlighted];
        [btnMenu addTarget:self action:@selector(onClickMenu) forControlEvents:UIControlEventTouchUpInside];
        btnMenu.alpha = 0.8;
        [mainView addSubview:btnMenu];
        
        btnBack = [[UIButton alloc]initWithFrame:CGRectMake(2, 2, MENU_BUTTON_WIDTH, MENU_BUTTON_WIDTH)];
        [btnBack setImage:[UIImage imageNamed:@"back1"] forState:UIControlStateNormal];
        [btnBack setImage:[UIImage imageNamed:@"back"] forState:UIControlStateHighlighted];
        [btnBack addTarget:self action:@selector(onClickBack) forControlEvents:UIControlEventTouchUpInside];
        btnBack.alpha = 0.8;
        btnBack.hidden = YES;
        [mainView addSubview:btnBack];
    }
    
    if (joystickBase.hidden == YES) {
        joystickBase.hidden = NO;
    }
    
    if (btnJump.hidden == YES) {
        btnJump.hidden = NO;
    }
    
    if (btnFire.hidden == YES) {
        btnFire.hidden = NO;
    }
    
    if (btnMenu.hidden == YES) {
        btnMenu.hidden = NO;
    }
    
    if (btnBack.hidden == NO) {
        btnBack.hidden = YES;
    }
    
    g_joystickType = JOYSTICK_MOVE;
}

-(void)onJumpDown
{
    if (g_inFadingDraw) {
        return;
    }

    if (!SMC::pKeyboard) {
        return;
    }

    SMC::pKeyboard->setKeyDown(SDLK_s, true);

    if (SMC::Game_Mode == SMC::MODE_LEVEL) {
        SMC::pKeyboard->Key_Down(SDLK_s);
    } else if (SMC::Game_Mode == SMC::MODE_OVERWORLD) {
        SMC::pKeyboard->Key_Down(SDLK_RETURN);
    }

    
    
    if (SMC::pKeyboard->isKeyPress(SDLK_s) && SMC::pKeyboard->isKeyPress(SDLK_SPACE)) {
        if (SMC::Game_Mode == SMC::MODE_LEVEL) {
            SMC::pLevel_Player->Get_Item(SMC::TYPE_STAR);
        }
    }
}

-(void)onJumpUp
{
    if (!SMC::pKeyboard) {
        return;
    }
    SMC::pKeyboard->setKeyDown(SDLK_s, false);

    if (SMC::Game_Mode == SMC::MODE_OVERWORLD) {
        SMC::pKeyboard->Key_Up(SDLK_RETURN);
    } else if (SMC::Game_Mode == SMC::MODE_LEVEL) {
        SMC::pKeyboard->Key_Up(SDLK_s);
    }
}

-(void)onClickFire
{
    if (g_inFadingDraw) {
        return;
    }

    if (!SMC::pKeyboard) {
        return;
    }
    
    SMC::pKeyboard->setKeyDown(SDLK_SPACE, true);

    if (SMC::Game_Mode == SMC::MODE_LEVEL) {
        SMC::pKeyboard->Key_Down(SDLK_SPACE);
    } else if (SMC::Game_Mode == SMC::MODE_OVERWORLD) {
        SMC::pKeyboard->Key_Down(SDLK_RETURN);
    }
    
    if (SMC::pKeyboard->isKeyPress(SDLK_s) && SMC::pKeyboard->isKeyPress(SDLK_SPACE)) {
        if (SMC::Game_Mode == SMC::MODE_LEVEL) {
            SMC::pLevel_Player->Get_Item(SMC::TYPE_STAR);
        }
    }
}

-(void)onClickFireUp
{
    SMC::pKeyboard->setKeyDown(SDLK_SPACE, false);
    
    if (SMC::Game_Mode == SMC::MODE_OVERWORLD) {
        SMC::pKeyboard->Key_Up(SDLK_RETURN);
    } else if (SMC::Game_Mode == SMC::MODE_LEVEL) {
        SMC::pKeyboard->Key_Up(SDLK_SPACE);
    }
}

-(void)onClickMenu
{
    if (SMC::Game_Mode == SMC::MODE_OVERWORLD) {
        SMC::pOverworld_Player->Action_Interact( SMC::INP_EXIT );
    } else {
        SMC::pLevel_Player->Action_Interact( SMC::INP_EXIT );
    }

    btnBack.hidden = NO;
}

-(void)onClickBack
{
    SMC::pMenuCore->Key_Down(SDLK_ESCAPE);
    btnBack.hidden = YES;
}

-(void)hideJoystick
{
    if (joystickBase == nil) {
        return;
    }
    
    joystickBase.hidden = YES;
    btnFire.hidden = YES;
    btnJump.hidden = YES;
    btnMenu.hidden = YES;
    g_joystickType = JOYSTICK_NONE;
}
@end


void showJoystick()
{
    if (g_joystickType == JOYSTICK_NONE) {
        [[MyDelegate sharedInstance]showJoystick];
    }
}

void hideJoystick()
{
    if (g_joystickType == JOYSTICK_NONE) {
        return;
    }

    [[MyDelegate sharedInstance]hideJoystick];
}

void showAds()
{
    if (!g_adsIsShown) {
        g_adsIsShown = true;
        [[MyDelegate sharedInstance]showAds];
    }
}

void closeAds()
{
    if (g_adsIsShown) {
        g_adsIsShown = false;
        [[MyDelegate sharedInstance]closeAds];
    }
}
