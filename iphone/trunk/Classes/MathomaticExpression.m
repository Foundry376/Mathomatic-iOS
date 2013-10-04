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

- (id)initWithCoder:(NSCoder *)decoder
{
    if (self = [super init]){
        [self setEquationText: [decoder decodeObject]];
        equationImage = [[UIImage imageWithData: [decoder decodeObject] scale: [[UIScreen mainScreen] scale]] retain];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)encoder
{
    [encoder encodeObject: equationText];
    [encoder encodeObject: UIImagePNGRepresentation(equationImage)];
}

- (void)setMathomaticText:(NSString*)t
{
    mathomaticText = [t retain];
    optimizeEquationImageParenthesis = YES;
    
    // Clean up some formatting so that the equation can be displayed nicely. This removes all of the 
    // # symbols used to differentiate variables from constants and also cleans up pi.
    if (equationText) [equationText release];
    equationText = [mathomaticText stringByReplacingOccurrencesOfString:@"pi#" withString:@"π"];
    equationText = [mathomaticText stringByReplacingOccurrencesOfString:@"pi" withString:@"π"];
    equationText = [equationText stringByReplacingOccurrencesOfString:@"e#" withString:@"exp"];
    equationText = [equationText stringByReplacingOccurrencesOfString:@"e" withString:@"E"];
    equationText = [equationText stringByReplacingOccurrencesOfString:@"Exp" withString:@"e"];
    equationText = [equationText stringByReplacingOccurrencesOfString:@"i#" withString:@"i"];
    equationText = [equationText stringByReplacingOccurrencesOfString:@"sign" withString:@"±1"];
    [equationText retain];
    
}

- (void)setEquationText:(NSString*)t
{
    [equationText release];
    equationText = [t retain];
    optimizeEquationImageParenthesis = NO;
    
    // Clean up some formatting on the equation text and make it match the 
    // raw text from Mathomatic
    if (mathomaticText) [mathomaticText release];
    mathomaticText = [mathomaticText stringByReplacingOccurrencesOfString:@"i" withString:@"i#"];
    mathomaticText = [equationText stringByReplacingOccurrencesOfString:@"π" withString:@"pi#"];
    mathomaticText = [mathomaticText stringByReplacingOccurrencesOfString:@"e" withString:@"e#"];
    mathomaticText = [mathomaticText stringByReplacingOccurrencesOfString:@"E" withString:@"e"];
    mathomaticText = [mathomaticText stringByReplacingOccurrencesOfString:@"±1" withString:@"sign"];
    [mathomaticText retain];
}

- (BOOL)isEquation
{
    return ([equationText rangeOfString: @"="].location != NSNotFound);
}

- (BOOL)isValidExpression
{
    // If the expression is an equation, we will call isValidExpression on each side of the equation.
    // Otherwise, just look for illegial characters and combinations of characters.
    if (![self isEquation]){
        NSString * illegialStartCharacters = @"*/)^=";
        NSString * illegialEndCharacters = @"*/^-+=.";
        
        // make sure the equation is not an empty string.
        if ([equationText length] == 0)
            return NO;
        
        // check the first character of the string
        NSString * s = [equationText substringToIndex: 1];
        if ([s rangeOfCharacterFromSet: [NSCharacterSet characterSetWithCharactersInString: illegialStartCharacters]].location != NSNotFound)
            return NO;
        
        // check the last character of the string. If it is a •, look at the second to last char.
        s = [equationText substringFromIndex: [equationText length] - 1];
        if ([s isEqualToString: @"•"])
            s = [equationText substringWithRange: NSMakeRange([equationText length] - 2, 1)];
        if ([s rangeOfCharacterFromSet: [NSCharacterSet characterSetWithCharactersInString: illegialEndCharacters]].location != NSNotFound)
            return NO;
            
        // now go through and look for illegial combinations of characters
        NSSet * illegialCombinations = [NSSet setWithObjects:   @"+/", @"+*", @"+^", @"+)",
                                                                @"-/", @"-*", @"-^", @"-)",
                                                                @"*/",        @"*^", @"*)",
                                                                @"^/", @"^*", @"^^", @"^)",
                                                                @"//", @"/*", @"/^", @"/)",
                                                                @"(/", @"(*", @"(^", @"()",
                                           @"..", @".+", @".-", @"./", @".*", @".^", @".)", nil];
                                           
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
    // return a sorted array of all of the variables used in the current expression.
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
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    float fontSize = 21;
    
    //if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)])
    //  fontSize *= [[UIScreen mainScreen] scale];

    // computing the image is expensive, so we only want to do it once.
    if (equationImage == nil)
    {
        NSMutableArray          * chunks = [NSMutableArray array];
        NSMutableString         * openBuffer = [NSMutableString stringWithCapacity: [equationText length]];
        UIView                  * equationView;
        EquationContainerView   * root;
        int                     location = 0;
        BOOL                    failed = NO;
        
        // First, we want to break the equation string into chunks based on delimiters like +, -, and variables.
        // The resulting array of chunks is used to create a tree of views that can be stacked and arranged.
        while (location < [equationText length]){
            NSString * c = [equationText substringWithRange: NSMakeRange(location, 1)];
            BOOL isOperator = ([c rangeOfCharacterFromSet:[NSCharacterSet characterSetWithCharactersInString:@"(xyzieπ)+-/*^=•"]].location == 0);
                
            if (isOperator){
                if ([openBuffer length] > 0){
                    [chunks addObject: openBuffer];
                    openBuffer = [NSMutableString stringWithCapacity: [equationText length]];
                }
                [chunks addObject: c];
                
            } else {
                [openBuffer appendString: c];
            }
            location ++;
        }
        // If there is a chunk remaining, add it before we analyze the result.
        if ([openBuffer length] > 0){
            [chunks addObject: openBuffer];
        }
    
        // create a containerView to be the base of our expression
        root = [[[EquationContainerView alloc] initWithParent: nil] autorelease];
        EquationContainerView * current = root;
        
        // create views for each of the chunks
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
            
            // The optimizeEquationImageParenthesis option allows you to remove the parenthesis from
            // container views with only one subview. This isn't desirable if the user actually typed
            // in the parenthesis, but it is good for auto-generated output from mathomatic, which contains
            // a lot of extra parenthesis.
            [root finalizeEquationTree: optimizeEquationImageParenthesis];
            [root finalizeTextSize: fontSize];
            [root finalizeFrame];
            failed = ![root isTreeLegal];
        }
        [root unlink];
        
        if (!failed){    
            equationView = root;
        } else {
            UILabel * l = [[[UILabel alloc] initWithFrame: CGRectZero] autorelease];
            [l setBackgroundColor: [UIColor clearColor]];
            [l setText: [equationText stringByReplacingOccurrencesOfString:@"•" withString:@""]];
            [l setFont: [UIFont fontWithName:@"Courier" size: fontSize]];
            
            CGSize textSize = [l.text sizeWithFont: l.font];
            [l setFrame: CGRectMake(5, 0, textSize.width, textSize.height)];
            
            equationView = l;
        }
        
        if (equationImage){
            [equationImage release];
            equationImage = nil;
        }
        CGSize imageSize = equationView.bounds.size;
        UIGraphicsBeginImageContextWithOptions(imageSize, NO, 2);
        [equationView.layer renderInContext: UIGraphicsGetCurrentContext()];
        equationImage = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
        [equationImage retain];
    }
    [pool release];
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

- (void)dealloc
{
    [mathomaticText release];
    [equationText release];
    [equationImage release];
    [super dealloc];
}

@end
