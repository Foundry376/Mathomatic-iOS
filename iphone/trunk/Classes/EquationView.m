//
//  EquationView.m
//  mathomatic
//
//  Created by Ben Gotow on 11/13/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "EquationView.h"
#import "EquationContainerView.h"
#import "EquationDivisionView.h"
#import "EquationExponentView.h"
#import "EquationTextView.h"
#import "EquationTypingBoxView.h"

@implementation EquationView

@synthesize editable;
@synthesize fontSize;
@synthesize equationHeight;

- (id)initWithEquation:(NSString*)e andFrame:(CGRect)frame andFontSize:(int)s
{
    if (self = [super initWithFrame: frame]) {
        [self setup];
        [self setScrollEnabled: NO];
        
        fontSize = s;
        equation = [e retain];
        [self assembleEquation];
    }
    return self;
}

- (void)setup
{
    [self setClipsToBounds: NO];
    [self setOpaque: NO];
    [self setBackgroundColor: [UIColor clearColor]];
    [self setShowsHorizontalScrollIndicator:YES];
    [self setShowsVerticalScrollIndicator: NO];
    [self setUserInteractionEnabled: YES];
}

- (void)setEquation:(NSString*)e
{
    [equation release];
    equation = [e retain];
    [self assembleEquation];
}

- (void)setFrame:(CGRect)frame
{
    [super setFrame: frame];
    if (frame.size.height == 0)
        [equationView setFrame: CGRectMake(5, 0, [equationView frame].size.width, [equationView frame].size.height)];
    else
        [equationView setFrame: CGRectMake(5, ([self frame].size.height - [equationView frame].size.height)/2, [equationView frame].size.width, [equationView frame].size.height)];
}

- (NSString*)equation
{
    if (equation == nil)
        equation = [[NSString stringWithString:@""] retain];
    return equation;
}

- (void)assembleEquation
{
    NSMutableArray      * chunks = [NSMutableArray array];
    NSMutableString     * openBuffer = [[NSMutableString alloc] initWithCapacity: [equation length]];
    int                   location = 0;
    BOOL                  failed = NO;
    
    // clear any existing views
    [[self subviews] makeObjectsPerformSelector:@selector(removeFromSuperview)];
    
    while (location < [equation length]){
        NSString * c = [equation substringWithRange: NSMakeRange(location, 1)];
        BOOL isOperator = ([c rangeOfCharacterFromSet:[NSCharacterSet characterSetWithCharactersInString:@"()+-/*^="]].location == 0);
            
        if (isOperator){
            if ([openBuffer length] > 0){
                [chunks addObject: openBuffer];
                
                [openBuffer release];
                openBuffer = [[NSMutableString alloc] initWithCapacity: [equation length]];
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

    EquationContainerView * root = [[[EquationContainerView alloc] initWithParent: nil] autorelease];
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
            id last = [current popInnerEquation];
            if (last){
                EquationExponentView * e = [[EquationExponentView alloc] initWithParent: current];
                [e addInnerEquation: last];
                [current addInnerEquation: e];
                [e release];
            } else {
                failed = YES;
                break;
            }
        } else if ([chunk isEqualToString: @"/"]){
            id last = [current popInnerEquation];
            EquationDivisionView * e = [[EquationDivisionView alloc] initWithParent: current];
            if (last){
                [e addInnerEquation: last];
                [current addInnerEquation: e];
                [e release];
            } else {
                failed = YES;
                break;
            }
        } else {
            EquationTextView * text = [[EquationTextView alloc] initWithText: chunk];
            [current addInnerEquation: text];
            [text release];
        }
    }
    if (!failed){
        if (editable){
            EquationTypingBoxView * box = [[EquationTypingBoxView alloc] init];
            if ([[[current innerEquations] lastObject] isKindOfClass: [EquationTextView class]] == YES){
                EquationContainerView * container = [[EquationContainerView alloc] initWithParent: current];
                [container addInnerEquation: [current popInnerEquation]];
                [container addInnerEquation: box];
                [current addInnerEquation: container];
            } else {
                [current addInnerEquation: box];
            }
        }
        
        // before we finish, iterate through the items in it and move things
        // around for the division and exponent views. Each of these two container subclasses need to
        // "swallow" the views following them. This can't be done when they are created because 
        // the next views haven't been created yet. It's easier done all at once at the end.
        [root finalizeEquationHierarchy];
        [root finalizeTextSize: fontSize];
        [root finalizeFrame];

        [equationView release];
        equationView = [root retain];
        equationHeight = [root frame].size.height;
        
    } else {
        UILabel * l = [[UILabel alloc] initWithFrame: CGRectZero];
        [l setBackgroundColor: [UIColor clearColor]];
        [l setText: equation];
        [l setTextColor: [UIColor redColor]];
        [l setFont: [UIFont fontWithName:@"Courier" size: fontSize]];
        
        CGSize textSize = [l.text sizeWithFont: l.font];
        [l setFrame: CGRectMake(5, ([self frame].size.height - textSize.height) / 2,textSize.width, textSize.height)];
        
        [equationView release];
        equationView = [l retain];
        [l release];
    }
    
    CGSize imageSize = equationView.bounds.size;
    
    UIGraphicsBeginImageContext(imageSize);
    [equationView.layer renderInContext: UIGraphicsGetCurrentContext()];
    UIImage * img = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    float maxHeight = [self frame].size.height;
    if (maxHeight == 0)
        maxHeight = imageSize.height;
    
    float scaleFactor = fminf(1, maxHeight/imageSize.height);
    if (![self isScrollEnabled])
        scaleFactor = fminf(scaleFactor, ([self frame].size.width-5)/imageSize.width);
        
    CGRect scaledFrame = CGRectMake(5, (maxHeight-imageSize.height*scaleFactor)/2, imageSize.width*scaleFactor, imageSize.height*scaleFactor);
    
    UIImageView * equationImageView = [[UIImageView alloc] initWithFrame: scaledFrame];
    [self addSubview: equationImageView];
    [equationImageView setImage: img];
    [equationImageView release];
    
    [self setContentSize: [equationImageView frame].size];
    if ([self isScrollEnabled]) [self scrollRectToVisible:CGRectMake(scaledFrame.size.width-10, 0, 10,10) animated:YES];
}

- (void)dealloc {
    [super dealloc];
}


@end
