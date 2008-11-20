//
//  KeyboardView.m
//  mathomatic
//
//  Created by Ben Gotow on 10/24/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "KeyboardView.h"
#import "MathomaticViewController.h"

@implementation KeyboardView

- (IBAction)buttonPressed:(UIButton*)button
{
    if (button == buttonMath){
    [delegate keyboardEntryPerform: [field equation]];
    } else if (button == buttonEnter){
        [delegate keyboardEntryComplete: [field equation]];
    }else if (button == buttonClear)
        [field setEquation: @""];
    else if (button == buttonDelete){
        if ([[field equation] length] > 0)
            [field setEquation: [[field equation] substringToIndex: [[field equation] length] - 1]];
    }else{
        NSString * c = [button titleForState: UIControlStateNormal];
        BOOL isVariable = ([c rangeOfCharacterFromSet:[NSCharacterSet characterSetWithCharactersInString:@"xyzπe"]].location == 0);
        BOOL isOperator = ([c rangeOfCharacterFromSet:[NSCharacterSet characterSetWithCharactersInString:@"()+-/*^="]].location == 0);
        
        if ((lastButtonWasVariable && !isOperator) && ([[field equation] length] > 0))
            [field setEquation: [[field equation] stringByAppendingFormat: @"*%@", c]];
        else
            [field setEquation: [[field equation] stringByAppendingString: c]];
            
        lastButtonWasVariable = isVariable;
    }
}

- (void)drawRect:(CGRect)rect {
    // Drawing code
}


- (void)dealloc {
    [super dealloc];
}


@end
