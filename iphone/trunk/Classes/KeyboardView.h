//
//  KeyboardView.h
//  mathomatic
//
//  Created by Ben Gotow on 10/24/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ExpressionScrollView.h"

@class MathomaticViewController;

@interface KeyboardView : UIView {
    IBOutlet MathomaticViewController * delegate;
    IBOutlet ExpressionScrollView * fieldScrollView;
    
    IBOutlet UIButton *button0;
    IBOutlet UIButton *button1;
    IBOutlet UIButton *button2;
    IBOutlet UIButton *button3;
    IBOutlet UIButton *button4;
    IBOutlet UIButton *button5;
    IBOutlet UIButton *button6;
    IBOutlet UIButton *button7;
    IBOutlet UIButton *button8;
    IBOutlet UIButton *button9;
    
    IBOutlet UIButton *buttonAdd;
    IBOutlet UIButton *buttonPlusMinus;
    IBOutlet UIButton *buttonMultiply;
    IBOutlet UIButton *buttonDivide;
    IBOutlet UIButton *buttonSubtract;
    
    IBOutlet UIButton *buttonE;
    IBOutlet UIButton *buttonPI;
    IBOutlet UIButton *buttonI;
    IBOutlet UIButton *buttonClear;
    IBOutlet UIButton *buttonDelete;
    IBOutlet UIButton *buttonMath;
    
    IBOutlet UIButton *buttonVarX;
    IBOutlet UIButton *buttonVarY;
    IBOutlet UIButton *buttonVarZ;
    
    IBOutlet UIButton *buttonPower;
    IBOutlet UIButton *buttonPeriod;
    IBOutlet UIButton *buttonParenOpen;
    IBOutlet UIButton *buttonParenClose;
    
    IBOutlet UIButton *buttonEnter;
    
    NSMutableString * fieldString;
}

@property (nonatomic, retain, getter=field) NSMutableString * fieldString;

- (IBAction)buttonPressed:(UIButton*)button;
- (void)clear;

- (BOOL)fieldIsBlank;
- (NSMutableString*)field;
- (void)setField:(NSString*)str;

@end
