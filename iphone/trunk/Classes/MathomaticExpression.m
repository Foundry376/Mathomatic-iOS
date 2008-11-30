//
//  MathomaticExpression.m
//  mathomatic
//
//  Created by Ben Gotow on 11/20/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MathomaticExpression.h"
#import "EquationContainerView.h"
#import "EquationDivisionView.h"
#import "EquationExponentView.h"
#import "EquationTextView.h"
#import "EquationTypingBoxView.h"

@implementation MathomaticExpression
@synthesize mathomaticText;
@synthesize equationText;
@synthesize equationImage;


+ expressionWithMathomaticText:(NSString*)text
{
    MathomaticExpression * e = [[MathomaticExpression alloc] init];
    [e setMathomaticText: text];
    return [e autorelease];
}

+ expressionWithEquationText:(NSString*)text
{
    MathomaticExpression * e = [[MathomaticExpression alloc] init];
    [e setEquationText: text];
    return [e autorelease];
}

- (void)setMathomaticText:(NSString*)t
{
    [mathomaticText release];
    mathomaticText = [t retain];
    optimizeEquationImageParenthesis = YES;
    
    // Clean up some formatting on the equation text and makeit match the 
    // raw text from Mathomatic
    if (equationText) [equationText release];
    equationText = [mathomaticText stringByReplacingOccurrencesOfString:@"pi#" withString:@"π"];
    equationText = [equationText stringByReplacingOccurrencesOfString:@"e#" withString:@"e"];
    equationText = [equationText stringByReplacingOccurrencesOfString:@"i#" withString:@"i"];
    equationText = [equationText stringByReplacingOccurrencesOfString:@"sign" withString:@"±1"];
    equationText = [equationText stringByReplacingOccurrencesOfString:@" " withString:@""];
    
    if ([equationText isEqualToString: @"Syntaxerror."])
        equationText = @"Invalid expression! Please check what you entered.";
    [equationText retain];
    
}

- (void)setEquationText:(NSString*)t
{
    [equationText release];
    equationText = [t retain];
    optimizeEquationImageParenthesis = NO;
    
    // Clean up some formatting on the equation text and makeit match the 
    // raw text from Mathomatic
    if (mathomaticText) [mathomaticText release];
    mathomaticText = [mathomaticText stringByReplacingOccurrencesOfString:@"i" withString:@"i#"];
    mathomaticText = [equationText stringByReplacingOccurrencesOfString:@"π" withString:@"pi#"];
    mathomaticText = [mathomaticText stringByReplacingOccurrencesOfString:@"e" withString:@"e#"];
    mathomaticText = [mathomaticText stringByReplacingOccurrencesOfString:@"±1" withString:@"sign"];
    [mathomaticText retain];
}

- (BOOL)isEquation
{
    return ([equationText rangeOfString: @"="].location != NSNotFound);
}

- (BOOL)isValidExpression
{
    if (![self isEquation]){
        // first look at the first and last characters
        NSString * illegialStartCharacters = @"*/)^=";
        NSString * illegialEndCharacters = @"*/^-+=";
        
        if ([equationText length] == 0)
            return NO;
            
        NSString * s = [equationText substringToIndex: 1];
        if ([s rangeOfCharacterFromSet: [NSCharacterSet characterSetWithCharactersInString: illegialStartCharacters]].location != NSNotFound)
            return NO;
        
        s = [equationText substringFromIndex: [equationText length] - 1];
        if ([s isEqualToString: @"•"])
            s = [equationText substringWithRange: NSMakeRange([equationText length] - 2, 1)];
        if ([s rangeOfCharacterFromSet: [NSCharacterSet characterSetWithCharactersInString: illegialEndCharacters]].location != NSNotFound)
            return NO;
            
        
        // now go through and look for illegial sets of characters
        NSSet * illegialCombinations = [NSSet setWithObjects:   @"+/", @"+*", @"+^",
                                                                @"-/", @"-*", @"-^",
                                                                @"*/",        @"*^",
                                                                @"^/", @"^*", @"^^",
                                                                @"//", @"/*", @"/^", nil];
        for (NSString * combination in illegialCombinations)
            if ([equationText rangeOfString: combination].location != NSNotFound)
                return NO;
        
    } else {
        if (![[self rhs] isValidExpression])
            return NO;
        if (![[self lhs] isValidExpression])
            return NO;
    }
    
    return YES;
}

- (NSArray*)equationVariables
{
    NSArray * possibleVariables = [NSArray arrayWithObjects:@"x",@"y",@"z", nil];
    NSMutableSet * variables = [NSMutableSet set];
    for (NSString * var in possibleVariables){
        if ([equationText rangeOfString:var].location != NSNotFound)
            [variables addObject: var];
    }
    return [[variables allObjects] sortedArrayUsingSelector:@selector(caseInsensitiveCompare:)];
}

- (UIImage*)equationImage
{
    if (equationImage == nil)
    {
        NSMutableArray          * chunks = [NSMutableArray array];
        NSMutableString         * openBuffer = [[NSMutableString alloc] initWithCapacity: [equationText length]];
        UIView                  * equationView;
        EquationContainerView   * root;
        int                     location = 0;
        BOOL                    failed = NO;
        
        while (location < [equationText length]){
            NSString * c = [equationText substringWithRange: NSMakeRange(location, 1)];
            BOOL isOperator = ([c rangeOfCharacterFromSet:[NSCharacterSet characterSetWithCharactersInString:@"(xyz)+-/*^=•"]].location == 0);
                
            if (isOperator){
                if ([openBuffer length] > 0){
                    [chunks addObject: openBuffer];
                    
                    [openBuffer release];
                    openBuffer = [[NSMutableString alloc] initWithCapacity: [equationText length]];
                }
                [chunks addObject: c];
                
            } else {
                [openBuffer appendString: c];
            }
            location ++;
        }
        if ([openBuffer length] > 0){
            [chunks addObject: openBuffer];
            [openBuffer release];
            openBuffer = nil;
        }

        root = [[[EquationContainerView alloc] initWithParent: nil] autorelease];
        EquationContainerView * current = root;
        
        // create views representing the chunks
        for (NSString * chunk in chunks)
        {
            if ([chunk isEqualToString: @"("]){
                EquationContainerView * c = [[[EquationContainerView alloc] initWithParent: current] autorelease];
                [c setShowOpenParenthesis:YES];
                [current addInnerEquation: c];
                current = c;
            } else if ([chunk isEqualToString: @")"]){
                [current setShowCloseParenthesis:YES];
                current = [current parent];
                if (current == nil){
                    failed = YES;
                    break;
                }
            } else if ([chunk isEqualToString: @"^"]){
                EquationExponentView * e = [[EquationExponentView alloc] initWithParent: current];
                [current addInnerEquation: e];
                [e release];
            } else if ([chunk isEqualToString: @"/"]){
                EquationDivisionView * e = [[EquationDivisionView alloc] initWithParent: current];
                [current addInnerEquation: e];
                [e release];
            } else if ([chunk isEqualToString: @"•"]){
                EquationTypingBoxView * box = [[EquationTypingBoxView alloc] init];
                if ([[[current innerEquations] lastObject] isKindOfClass: [EquationTextView class]] == YES){
                    EquationContainerView * container = [[EquationContainerView alloc] initWithParent: current];
                    [container addInnerEquation: [current popInnerEquation]];
                    [container addInnerEquation: box];
                    [current addInnerEquation: container];
                } else {
                    [current addInnerEquation: box];
                }
            } else {
                EquationTextView * text = [[EquationTextView alloc] initWithText: chunk];
                [current addInnerEquation: text];
                [text release];
            }
        }
        
        if (!failed){
            // before we finish, iterate through the items in it and move things
            // around for the division and exponent views. Each of these two container subclasses need to
            // "swallow" the views following them. This can't be done when they are created because 
            // the next views haven't been created yet. It's easier done all at once at the end.
            [root finalizeEquationTree: optimizeEquationImageParenthesis];
            [root finalizeTextSize: 21];
            [root finalizeFrame];
            failed = ![root isTreeLegal];
        }
        
        if (!failed){    
            equationView = root;
        } else {
            UILabel * l = [[[UILabel alloc] initWithFrame: CGRectZero] autorelease];
            [l setBackgroundColor: [UIColor clearColor]];
            [l setText: [equationText stringByReplacingOccurrencesOfString:@"•" withString:@""]];
            [l setFont: [UIFont fontWithName:@"Courier" size: 21.0]];
            
            CGSize textSize = [l.text sizeWithFont: l.font];
            [l setFrame: CGRectMake(5, 0, textSize.width, textSize.height)];
            
            equationView = l;
        }
        
        CGSize imageSize = equationView.bounds.size;
        UIGraphicsBeginImageContext(imageSize);
        [equationView.layer renderInContext: UIGraphicsGetCurrentContext()];
        equationImage = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
        [equationImage retain];
    }
    return equationImage;
}

- (MathomaticExpression*)lhs
{
    if (![self isEquation])
        return nil;
 
    int r = [mathomaticText rangeOfString:@"="].location;
    return [MathomaticExpression expressionWithMathomaticText: [mathomaticText substringToIndex: r]];
}

- (MathomaticExpression*)rhs
{
    if (![self isEquation])
        return nil;
 
    int r = [mathomaticText rangeOfString:@"="].location;
    return [MathomaticExpression expressionWithMathomaticText: [mathomaticText substringFromIndex: r + 1]];
}

@end
