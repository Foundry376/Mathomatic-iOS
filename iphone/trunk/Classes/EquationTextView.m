//
//  EquationTextView.m
//  mathomatic
//
//  Created by Ben Gotow on 11/14/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "EquationTextView.h"

@implementation EquationTextView


- (id)initWithText:(NSString*)t 
{
    if (self = [super initWithFrame:CGRectZero]) 
    {
        text = [[UILabel alloc] initWithFrame: CGRectZero];
        [text setText: [t stringByTrimmingCharactersInSet: [NSCharacterSet whitespaceCharacterSet]]];
        [text setBackgroundColor: [UIColor clearColor]];
        [self addSubview: text];
    }
    return self;
}

- (void)finalizeEquationHierarchy
{

}

- (void)finalizeTextSize:(int)parentTextSize
{
    [text setFont: [UIFont fontWithName:@"Courier" size: parentTextSize]];
}

- (void)finalizeFrame
{
    CGSize textSize = [text.text sizeWithFont: text.font];
    CGRect frame = CGRectMake(0,0, textSize.width, textSize.height);
    [self setFrame: frame];
    [text setFrame: frame];
}

- (NSString*)description
{
    return [text text];
}

- (void)dealloc 
{
    [text release];
    [super dealloc];
}


@end
