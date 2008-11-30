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

@synthesize fieldString;

- (IBAction)buttonPressed:(UIButton*)button
{
    BOOL fieldChanged = NO;
    
    if (fieldString == nil)
        fieldString = [[NSMutableString alloc] init];
        
    if (button == buttonMath){
        [delegate keyboardEntryPerform];
    
    } else if (button == buttonEnter){
        [delegate keyboardEntryComplete];
    
    } else if (button == buttonClear){
        [self clear];
    
    } else if (button == buttonDelete){
        if ([fieldString length] > 0){
            [fieldString deleteCharactersInRange: NSMakeRange([fieldString length]-1, 1)];
            fieldChanged = YES;
        }
        
    } else {
        BOOL previousIsVariable;
        BOOL typedIsVariable;
        
        NSString * typed = [button titleForState: UIControlStateNormal];
        typedIsVariable = ([typed rangeOfCharacterFromSet:[NSCharacterSet characterSetWithCharactersInString:@"xyzπie"]].location == 0);
        
        if ([fieldString length] > 0){
            NSString * previous = [fieldString substringFromIndex: [fieldString length] - 1];
            previousIsVariable = ([previous rangeOfCharacterFromSet:[NSCharacterSet characterSetWithCharactersInString:@"xyzπie"]].location == 0);
        } else {
            previousIsVariable = NO;
        }
        
        if (previousIsVariable && typedIsVariable)
            [fieldString appendFormat: @"*%@", typed];
        else
            [fieldString appendString: typed];
        fieldChanged = YES;
    }
    
    if (fieldChanged)
        [fieldScrollView setExpression: [MathomaticExpression expressionWithEquationText: [fieldString stringByAppendingString: @"•"]]];
}

- (void)clear
{
    [fieldScrollView setExpression: nil];
    [fieldString setString: @""];
}

- (BOOL)fieldIsBlank
{
    return ([fieldString length] == 0);
}

- (NSMutableString*)field
{
    return [NSMutableString stringWithString: fieldString];
}

- (void)setField:(NSString*)str
{
    [fieldString setString: str];
    [fieldScrollView setExpression: [MathomaticExpression expressionWithEquationText: [fieldString stringByAppendingString: @"•"]]];
}

- (void)dealloc {
    [fieldString release];
    [super dealloc];
}


@end
