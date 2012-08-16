//
//  CGJoystick.m
//  Pal_iOS
//
//  Created by 王 佳 on 12-7-25.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "CGJoystick.h"
#import <QuartzCore/QuartzCore.h>
#include "core/game_core.h"
#include "core/global_game.h"
#include "input/keyboard.h"

@implementation CGJoystick

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        self.backgroundColor = [UIColor clearColor];
    }
    return self;
}

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{

    for (UITouch* touch in touches) {
        CGPoint point = [touch locationInView:self];
        CGRect rect = self.bounds;
        if (point.x >= 0 && point.x <= rect.size.width
            && point.y >= 0 && point.y <= rect.size.height) {
            int dir = [self getDirByPoint:point];
            if (stickDir != dir) {
                [self setNeedsDisplay];
                stickDir = dir;
            }
        }
    }
    
    [self doMove:stickDir];
}

-(void)doMove:(int)dir
{
    if (!SMC::pKeyboard) {
        return;
    }

    if (SMC::Game_Mode == SMC::MODE_OVERWORLD || SMC::Game_Mode == SMC::MODE_LEVEL) {
        switch (dir) {
            case JS_DIR_UP:
                SMC::pKeyboard->setKeyDown(SDLK_UP, true);
                SMC::pKeyboard->Key_Down(SDLK_UP);

                SMC::pKeyboard->Key_Up(SDLK_DOWN);
                SMC::pKeyboard->Key_Up(SDLK_LEFT);
                SMC::pKeyboard->Key_Up(SDLK_RIGHT);

                break;
            case JS_DIR_DOWN:
                SMC::pKeyboard->setKeyDown(SDLK_DOWN, true);
                SMC::pKeyboard->Key_Down(SDLK_DOWN);
                
                SMC::pKeyboard->Key_Up(SDLK_UP);
                SMC::pKeyboard->Key_Up(SDLK_LEFT);
                SMC::pKeyboard->Key_Up(SDLK_RIGHT);
                break;
            case JS_DIR_LEFT:
                SMC::pKeyboard->setKeyDown(SDLK_LEFT, true);
                SMC::pKeyboard->Key_Down(SDLK_LEFT);
                
                SMC::pKeyboard->Key_Up(SDLK_DOWN);
                SMC::pKeyboard->Key_Up(SDLK_UP);
                SMC::pKeyboard->Key_Up(SDLK_RIGHT);

                break;
            case JS_DIR_RIGHT:
                SMC::pKeyboard->setKeyDown(SDLK_RIGHT, true);
                SMC::pKeyboard->Key_Down(SDLK_RIGHT);
                
                SMC::pKeyboard->Key_Up(SDLK_DOWN);
                SMC::pKeyboard->Key_Up(SDLK_LEFT);
                SMC::pKeyboard->Key_Up(SDLK_UP);
                break;       
            default:
                SMC::pKeyboard->setKeyDown(SDLK_UP, false);
                SMC::pKeyboard->setKeyDown(SDLK_DOWN, false);
                SMC::pKeyboard->setKeyDown(SDLK_LEFT, false);
                SMC::pKeyboard->setKeyDown(SDLK_RIGHT, false);
                SMC::pKeyboard->Key_Up(SDLK_UP);
                SMC::pKeyboard->Key_Up(SDLK_DOWN);
                SMC::pKeyboard->Key_Up(SDLK_LEFT);
                SMC::pKeyboard->Key_Up(SDLK_RIGHT);
                break;
        }
    }
}

-(int)getDirByPoint:(CGPoint)point
{
    int dir = 0;
    CGSize size = self.frame.size;
    CGPoint centerPoint = CGPointMake(size.width / 2, size.height / 2);
    int diffX = point.x - centerPoint.x;
    int diffY = centerPoint.y - point.y;
    
    if (diffX == 0) {
        diffX = 1;
    }
    
    if (diffY == 0) {
        diffY = 1;
    }

    int absX = abs(diffX);
    int absY = abs(diffY);
#if 0
    if (diffY > 0 && (absX <= absY)) {
        dir = JS_DIR_UP;
    } else if (diffY < 0 && absX <= absY) {
        dir = JS_DIR_DOWN;
    } else if (diffX > 0 && absX >= absY) {
        dir = JS_DIR_RIGHT;
    } else if (diffX < 0 && absX >= absY) {
        dir = JS_DIR_LEFT;
    } else {
        dir = 0;
    }
#else
    float tan30 = tanf(M_PI / 180.0 * 60);
    float tanmy = absY / absX;
    if (diffY > 0 && (tanmy > tan30)) {
        dir = JS_DIR_UP;
    } else if (diffY < 0 && (tanmy > tan30)) {
        dir = JS_DIR_DOWN;
    } else if (diffX > 0 && (tanmy <= tan30)) {
        dir = JS_DIR_RIGHT;
    } else if (diffX < 0 && (tanmy <= tan30)) {
        dir = JS_DIR_LEFT;
    } else {
        dir = 0;
    }
#endif
    
    return dir;
}

-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UITouch* touch in touches) {
        CGPoint point = [touch locationInView:self];
        CGRect rect = self.bounds;
        if (point.x >= 0 && point.x <= rect.size.width
            && point.y >= 0 && point.y <= rect.size.height) {
            int dir = [self getDirByPoint:point];
            if (stickDir != dir) {
                [self setNeedsDisplay];
                stickDir = dir;
            }
        }
    }
    
    [self doMove:stickDir];
}

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (stickDir != 0) {
        [self setNeedsDisplay];
    }
    stickDir = 0;
    [self doMove:0];
}

-(void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (stickDir != 0) {
        [self setNeedsDisplay];
    }
    stickDir = 0;
}


// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    stickBase = [UIImage imageNamed:@"jsbg"];
    stickUp = [UIImage imageNamed:@"jsup"];
    stickDown = [UIImage imageNamed:@"jsdown"];
    stickLeft = [UIImage imageNamed:@"jsleft"];
    stickRight = [UIImage imageNamed:@"jsright"];

    // Drawing code
//    CGContextRef context = UIGraphicsGetCurrentContext();
    [stickBase drawInRect:rect];
    
    int width = self.bounds.size.width;

    switch (stickDir) {
        case JS_DIR_UP:
            [stickUp drawInRect:CGRectMake(58 * width / 150, 15 * width / 150, 35 * width / 150, 90 * width / 150)];
            break;
        case JS_DIR_DOWN:
            [stickDown drawInRect:CGRectMake(59 * width / 150, 68 * width / 150, 35 * width / 150, 90 * width / 150)];
            break;
        case JS_DIR_LEFT:
            [stickLeft drawInRect:CGRectMake(13 * width / 150, 60 * width / 150, 90 * width / 150, 35 * width / 150)];
            break;
        case JS_DIR_RIGHT:
            [stickRight drawInRect:CGRectMake(65 * width / 150, 60 * width / 150, 90 * width / 150, 35 * width / 150)];
            break;
        default:
            break;
    }
}


@end
