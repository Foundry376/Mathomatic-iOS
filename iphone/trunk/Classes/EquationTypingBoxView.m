//
//  EquationTypingBoxView.m
//  mathomatic
//
//  Created by Ben Gotow on 11/15/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "EquationTypingBoxView.h"


@implementation EquationTypingBoxView


- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        [self setBackgroundColor: [UIColor grayColor]];
    }
    return self;
}


- (void)finalizeEquationHierarchy
{

}

- (void)finalizeTextSize:(int)p
{
    parentTextSize = p;
}

- (void)finalizeFrame
{
    UIFont * font = [UIFont fontWithName:@"Courier" size: parentTextSize];
    CGSize textSize = [[NSString stringWithString:@"0"] sizeWithFont: font];
    CGRect frame = CGRectMake(0,0, textSize.width, textSize.height);
    [self setFrame: frame];
}

- (void)dealloc {
    [super dealloc];
}


@end
