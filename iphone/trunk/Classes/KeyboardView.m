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
        BOOL addMultiplication;
        NSString * typed = nil;
        
        if (button == buttonAns){
            if ([delegate lastExpression] == nil) return;
            typed = [[delegate lastExpression] equationText];
        }else
            typed = [button titleForState: UIControlStateNormal];
            
        
        if ([fieldString length] > 0){
            NSString * previous = [fieldString substringFromIndex: [fieldString length] - 1];
            
            BOOL typedIsNotOperator = ([typed rangeOfCharacterFromSet:[NSCharacterSet characterSetWithCharactersInString:@"+-*/^=)"]].location != 0);
            BOOL typedIsOpenParen = ([typed rangeOfString:@"("].location == 0);
            BOOL previousIsNumberic = ([previous rangeOfCharacterFromSet:[NSCharacterSet characterSetWithCharactersInString:@"0123456789"]].location == 0);
            BOOL previousIsVariable = ([previous rangeOfCharacterFromSet:[NSCharacterSet characterSetWithCharactersInString:@"xyzπie"]].location == 0);
            
            addMultiplication = ((typedIsNotOperator && previousIsVariable) || (typedIsOpenParen && previousIsNumberic));
            
            // for some reason mathomatic can't handle two blocks of parenthesis without a * inbetween. (5)(5) != 25?
            if (([previous isEqualToString: @")"]) && ([typed isEqualToString:@"("]))
                addMultiplication = YES;
        
        } else {
            addMultiplication = NO;
        }
        
        if (addMultiplication)
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

- (void)setField:(MathomaticExpression*)e
{
    if (fieldString == nil)
        fieldString = [[NSMutableString alloc] init];
        
    [fieldString setString: [e equationText]];
    MathomaticExpression * f = [MathomaticExpression expressionWithEquationText: [fieldString stringByAppendingString: @"•"]];
    [fieldScrollView setExpression: f];
}

- (void)dealloc {
    [fieldString release];
    [super dealloc];
}


@end
